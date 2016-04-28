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

const string db_table_name = "events";
const string db_column_id = "id";
const string db_column_event = "event";

Storage::Storage(string db_name) {
  sqlite3 *db;
  char *err_msg = 0;
  int rc;

  // Open Database connection
  rc = sqlite3_open((const char *) db_name.c_str(), &db);
  if (rc) {
    cerr << "FATAL: Cannot open database: " << sqlite3_errmsg(db) << endl;
    exit(1);
  }
  this->m_db_name = db_name;
  this->m_db = db;

  // WAL query
  string wal_query = "PRAGMA journal_mode=WAL;";
  rc = sqlite3_exec(this->m_db, (const char *) wal_query.c_str(), NULL, NULL, &err_msg);
  if (rc != SQLITE_OK) {
    cerr << "FATAL: Cannot enable WAL: " << err_msg << endl;
    sqlite3_free(err_msg);
    exit(1);
  }

  // Create query
  string create_query = 
    "CREATE TABLE IF NOT EXISTS " + db_table_name + "(" +
      db_column_id + " INTEGER PRIMARY KEY, " +
      db_column_event + " STRING" +
    ");";

  // Make new events table
  rc = sqlite3_exec(this->m_db, (const char *) create_query.c_str(), NULL, NULL, &err_msg);
  if (rc != SQLITE_OK) {
    cerr << "FATAL: Cannot create events table: " << err_msg << endl;
    sqlite3_free(err_msg);
    exit(1);
  }

  // Insert query
  string insert_query = 
    "INSERT INTO " + db_table_name + "(" +
      db_column_event +
    ") values(?1);";

  // Prepare insert statement
  rc = sqlite3_prepare_v2(this->m_db, (const char *) insert_query.c_str(), -1, &this->m_add_stmt, NULL);
  if (rc != SQLITE_OK) {
    cerr << "FATAL: Cannot prepare event insert statement: " << rc << endl;
    exit(1);
  }
}

Storage::~Storage() {
  sqlite3_finalize(this->m_add_stmt);
  sqlite3_close(this->m_db);
}

// --- INSERT

void Storage::insert_payload(Payload payload) {
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
  }

  rc = sqlite3_reset(this->m_add_stmt);
  if (rc != SQLITE_OK) {
    cerr << "ERROR: Failed to reset add_stmt after insert: " << rc << endl;
  }
}

// --- SELECT

static int select_callback(void *data, int argc, char **argv, char **az_col_name) {
   int i, id;
   list<EventRow>* data_list = (list<EventRow>*)data;
   Payload event;

   for (i = 0; i < argc; i++) {
      if (az_col_name[i] == db_column_id) {
        id = std::stoi(argv[i] ? argv[i] : "-1");
      } else if (az_col_name[i] == db_column_event) {
        event = Utils::deserialize_json_str(argv[i] ? argv[i] : "");
      }
   }

   EventRow event_row(id, event);
   data_list->push_back(event_row);
   return 0;
}

void Storage::select_all_event_rows(list<EventRow>* event_list) {
  int rc;
  char *err_msg = 0;

  string select_all_query =
    "SELECT * FROM " + db_table_name + ";";

  rc = sqlite3_exec(this->m_db, (const char *) select_all_query.c_str(), select_callback, (void*)event_list, &err_msg);
  if (rc != SQLITE_OK) {
    cerr << "ERROR: Failed to execute select_all_query: " << rc << "; " << err_msg << endl;
    sqlite3_free(err_msg);
  }
}

void Storage::select_event_row_range(list<EventRow>* event_list, int range) {
  int rc;
  char *err_msg = 0;

  string select_range_query =
    "SELECT * FROM " + db_table_name + " " +
    "ORDER BY " + db_column_id + " ASC LIMIT " + std::to_string(range) + ";";

  rc = sqlite3_exec(this->m_db, (const char *) select_range_query.c_str(), select_callback, (void*)event_list, &err_msg);
  if (rc != SQLITE_OK) {
    cerr << "ERROR: Failed to execute select_range_query: " << rc << "; " << err_msg << endl;
    sqlite3_free(err_msg);
  }
}

// --- DELETE

void Storage::delete_all_event_rows() {
  int rc;
  char *err_msg = 0;

  string delete_all_query =
    "DELETE FROM " + db_table_name + ";";

  rc = sqlite3_exec(this->m_db, (const char *) delete_all_query.c_str(), NULL, NULL, &err_msg);
  if (rc != SQLITE_OK) {
    cerr << "ERROR: Failed to execute delete_all_query: " << rc << "; " << err_msg << endl;
    sqlite3_free(err_msg);
  }
}

void Storage::delete_event_row_ids(list<int>* id_list) {
  int rc;
  char *err_msg = 0;

  string delete_range_query =
    "DELETE FROM " + db_table_name + " " +
    "WHERE " + db_column_id + " in(" + Utils::int_list_to_string(id_list, ",") + ");";

  rc = sqlite3_exec(this->m_db, (const char *) delete_range_query.c_str(), NULL, NULL, &err_msg);
  if (rc != SQLITE_OK) {
    cerr << "ERROR: Failed to execute delete_range_query: " << rc << "; " << err_msg << endl;
    sqlite3_free(err_msg);
  }
}

// --- Getters

string Storage::get_db_name() {
  return this->m_db_name;
}
