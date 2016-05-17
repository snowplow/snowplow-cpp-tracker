/*
Copyright (c) 2016 Snowplow Analytics Ltd. All rights reserved.

This program is licensed to you under the Apache License Version 2.0,
and you may not use this file except in compliance with the Apache License Version 2.0.
You may obtain a copy of the Apache License Version 2.0 at http://www.apache.org/licenses/LICENSE-2.0.

Unless required by applicable law or agreed to in writing,
software distributed under the Apache License Version 2.0 is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the Apache License Version 2.0 for the specific language governing permissions and limitations there under.
*/

#include "storage.hpp"

const string db_table_events = "events";
const string db_column_events_id = "id";
const string db_column_events_data = "data";

const string db_table_session = "sessions";
const string db_column_session_id = "id";
const string db_column_session_data = "data";

// --- Static Singleton Access

Storage *Storage::m_instance = 0;
mutex Storage::m_db_get;

Storage *Storage::instance(const string & db_name) {
  lock_guard<mutex> guard(m_db_get);
  if (!m_instance) {
    m_instance = new Storage(db_name);
  }
  return m_instance;
}

void Storage::close() {
  delete(m_instance);
  m_instance = 0;
}

// --- Constructor & Destructor

Storage::Storage(const string & db_name) {
  sqlite3 *db;
  char *err_msg = 0;
  int rc;

  // Open Database connection
  rc = sqlite3_open((const char *) db_name.c_str(), &db);
  if (rc) {
    throw runtime_error((string) "FATAL: Cannot open database: " + sqlite3_errmsg(db));
  }
  this->m_db_name = db_name;
  this->m_db = db;

  // WAL query
  string wal_query = "PRAGMA journal_mode=WAL;";
  rc = sqlite3_exec(this->m_db, (const char *) wal_query.c_str(), NULL, NULL, &err_msg);
  if (rc != SQLITE_OK) {
    string err = "FATAL: Cannot enable WAL: " + string(err_msg);
    sqlite3_free(err_msg);
    throw runtime_error(err);
  }

  // Create events table query
  string create_events_query = 
    "CREATE TABLE IF NOT EXISTS " + db_table_events + "(" +
      db_column_events_id + " INTEGER PRIMARY KEY, " +
      db_column_events_data + " STRING" +
    ");";

  // Make new events table
  rc = sqlite3_exec(this->m_db, (const char *) create_events_query.c_str(), NULL, NULL, &err_msg);
  if (rc != SQLITE_OK) {
    cerr << "FATAL: Cannot create events table: " << err_msg << endl;
    string err = "FATAL: Cannot create events table: " + string(err_msg);
    sqlite3_free(err_msg);
    throw runtime_error(err);
  }

  // Create session table query
  string create_sessions_query = 
    "CREATE TABLE IF NOT EXISTS " + db_table_session + "(" +
      db_column_session_id + " INTEGER PRIMARY KEY, " +
      db_column_session_data + " STRING" +
    ");";

  // Make new session table
  rc = sqlite3_exec(this->m_db, (const char *) create_sessions_query.c_str(), NULL, NULL, &err_msg);
  if (rc != SQLITE_OK) {
    cerr << "FATAL: Cannot create sessions table: " << err_msg << endl;
    string err = "FATAL: Cannot create sessions table: " + string(err_msg);
    sqlite3_free(err_msg);
    throw runtime_error(err);
  }

  // Insert query
  string insert_query = 
    "INSERT INTO " + db_table_events + "(" +
      db_column_events_data +
    ") values(?1);";

  // Prepare insert statement
  rc = sqlite3_prepare_v2(this->m_db, (const char *) insert_query.c_str(), -1, &this->m_add_stmt, NULL);
  if (rc != SQLITE_OK) {
    throw runtime_error("FATAL: Cannot prepare event insert statement: " + std::to_string(rc));
  }
}

Storage::~Storage() {
  sqlite3_finalize(this->m_add_stmt);
  sqlite3_close(this->m_db);
}

// --- INSERT

void Storage::insert_payload(Payload payload) {
  lock_guard<mutex> guard(this->m_db_access);
  
  int rc;

  string payload_str = Utils::serialize_payload(payload);

  rc = sqlite3_bind_text(this->m_add_stmt, 1, payload_str.c_str(), payload_str.length(), SQLITE_STATIC);
  if (rc != SQLITE_OK) {
    cerr << "ERROR: Failed to bind payload to statement: " << rc << endl;
    return;
  }

  rc = sqlite3_step(this->m_add_stmt);
  if (rc != SQLITE_DONE) {
    cerr << "ERROR: Failed to execute add_stmt: " << rc << endl;
    return;
  }

  rc = sqlite3_reset(this->m_add_stmt);
  if (rc != SQLITE_OK) {
    cerr << "ERROR: Failed to reset add_stmt after insert: " << rc << endl;
    return;
  }
}

void Storage::insert_update_session(json session_data) {
  lock_guard<mutex> guard(this->m_db_access);
  
  int rc;
  sqlite3_stmt *insert_stmt;

  string insert_query = 
    "INSERT OR REPLACE INTO " + db_table_session + "(" +
      db_column_session_id + "," + db_column_session_data +
    ") values(?1, ?2);";

  rc = sqlite3_prepare_v2(this->m_db, (const char *) insert_query.c_str(), -1, &insert_stmt, NULL);
  if (rc != SQLITE_OK) {
    cerr << "ERROR: Cannot prepare session insert statement: " << std::to_string(rc) << endl;
    return;
  }

  rc = sqlite3_bind_int(insert_stmt, 1, 1);
  if (rc != SQLITE_OK) {
    cerr << "ERROR: Failed to bind id to statement: " << rc << endl;
    return;
  }

  string session_data_str = session_data.dump();
  rc = sqlite3_bind_text(insert_stmt, 2, session_data_str.c_str(), session_data_str.length(), SQLITE_STATIC);
  if (rc != SQLITE_OK) {
    cerr << "ERROR: Failed to bind data to statement: " << rc << endl;
    return;
  }

  rc = sqlite3_step(insert_stmt);
  if (rc != SQLITE_DONE) {
    cerr << "ERROR: Failed to execute insert_stmt: " << rc << endl;
    return;
  }

  rc = sqlite3_reset(insert_stmt);
  if (rc != SQLITE_OK) {
    cerr << "ERROR: Failed to reset insert_stmt after insert: " << rc << endl;
    return;
  }

  sqlite3_finalize(insert_stmt);
}

// --- SELECT

static int select_event_callback(void *data, int argc, char **argv, char **az_col_name) {
   int i, id;
   list<Storage::EventRow>* data_list = (list<Storage::EventRow>*)data;
   Payload event;

   for (i = 0; i < argc; i++) {
      if (az_col_name[i] == db_column_events_id) {
        id = std::stoi(argv[i] ? argv[i] : "-1");
      } else if (az_col_name[i] == db_column_events_data) {
        event = Utils::deserialize_json_str(argv[i] ? argv[i] : "");
      }
   }

   Storage::EventRow event_row;
   event_row.id = id;
   event_row.event = event;
   data_list->push_back(event_row);
   return 0;
}

void Storage::select_all_event_rows(list<Storage::EventRow>* event_list) {
  lock_guard<mutex> guard(this->m_db_access);

  int rc;
  char *err_msg = 0;

  string select_all_query =
    "SELECT * FROM " + db_table_events + ";";

  rc = sqlite3_exec(this->m_db, (const char *) select_all_query.c_str(), select_event_callback, (void*)event_list, &err_msg);
  if (rc != SQLITE_OK) {
    cerr << "ERROR: Failed to execute select_all_query: " << rc << "; " << err_msg << endl;
    sqlite3_free(err_msg);
  }
}

void Storage::select_event_row_range(list<Storage::EventRow>* event_list, int range) {
  lock_guard<mutex> guard(this->m_db_access);

  int rc;
  char *err_msg = 0;

  string select_range_query =
    "SELECT * FROM " + db_table_events + " " +
    "ORDER BY " + db_column_events_id + " ASC LIMIT " + std::to_string(range) + ";";

  rc = sqlite3_exec(this->m_db, (const char *) select_range_query.c_str(), select_event_callback, (void*)event_list, &err_msg);
  if (rc != SQLITE_OK) {
    cerr << "ERROR: Failed to execute select_range_query: " << rc << "; " << err_msg << endl;
    sqlite3_free(err_msg);
  }
}

static int select_session_callback(void *data, int argc, char **argv, char **az_col_name) {
   int i;
   list<json>* data_list = (list<json>*)data;
   json session_data;

   for (i = 0; i < argc; i++) {
      if (az_col_name[i] == db_column_session_data) {
        session_data = json::parse(argv[i] ? argv[i] : "");
        break;
      }
   }

   data_list->push_back(session_data);
   return 0;
}

void Storage::select_all_session_rows(list<json>* session_list) {
  lock_guard<mutex> guard(this->m_db_access);

  int rc;
  char *err_msg = 0;

  string select_all_query =
    "SELECT * FROM " + db_table_session + ";";

  rc = sqlite3_exec(this->m_db, (const char *) select_all_query.c_str(), select_session_callback, (void*)session_list, &err_msg);
  if (rc != SQLITE_OK) {
    cerr << "ERROR: Failed to execute select_all_query: " << rc << "; " << err_msg << endl;
    sqlite3_free(err_msg);
  }
}

// --- DELETE

void Storage::delete_all_event_rows() {
  lock_guard<mutex> guard(this->m_db_access);

  int rc;
  char *err_msg = 0;

  string delete_all_query =
    "DELETE FROM " + db_table_events + ";";

  rc = sqlite3_exec(this->m_db, (const char *) delete_all_query.c_str(), NULL, NULL, &err_msg);
  if (rc != SQLITE_OK) {
    cerr << "ERROR: Failed to execute delete_all_query: " << rc << "; " << err_msg << endl;
    sqlite3_free(err_msg);
  }
}

void Storage::delete_event_row_ids(list<int>* id_list) {
  lock_guard<mutex> guard(this->m_db_access);
  
  int rc;
  char *err_msg = 0;

  string delete_range_query =
    "DELETE FROM " + db_table_events + " " +
    "WHERE " + db_column_events_id + " in (" + Utils::int_list_to_string(id_list, ",") + ");";

  rc = sqlite3_exec(this->m_db, (const char *) delete_range_query.c_str(), NULL, NULL, &err_msg);
  if (rc != SQLITE_OK) {
    cerr << "ERROR: Failed to execute delete_range_query: " << rc << "; " << err_msg << endl;
    sqlite3_free(err_msg);
  }
}

void Storage::delete_all_session_rows() {
  lock_guard<mutex> guard(this->m_db_access);

  int rc;
  char *err_msg = 0;

  string delete_all_query =
    "DELETE FROM " + db_table_session + ";";

  rc = sqlite3_exec(this->m_db, (const char *) delete_all_query.c_str(), NULL, NULL, &err_msg);
  if (rc != SQLITE_OK) {
    cerr << "ERROR: Failed to execute delete_all_query: " << rc << "; " << err_msg << endl;
    sqlite3_free(err_msg);
  }
}

// --- Getters

string Storage::get_db_name() {
  return this->m_db_name;
}
