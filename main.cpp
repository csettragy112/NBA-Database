/* 
Program name: nba_database.cpp
Author: Captain Jack Sparrow
Date last updated: 06/05/2021
Purpose: NBA Database
*/

#include <iostream>
#include <string>
#include <sqlite3.h>

using namespace std;

/* Global Variables */
sqlite3 *db;
char* zerrmsg = 0;
const char* data = "callback";

/* Logic Functions */

void loadSampleData();
int mainMenu();

void listDivisions();
void listTeams();
void addTeam();
void addPlayer();
void addDivision();
void addCoach();
void viewTeamRoster();
void viewGameInfo();
void addGameInfo();

/* Main Driver */

int main() {
  string sql = "";
  sqlite3_stmt *res;
  string lastmsg;
  int rc;
  int choice;

  // 1. Open Database
  rc = sqlite3_open("nba.db", &db);
  if (rc) {  // Throw Error
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    printf("Make sure database file is in same directory as the program, and db name is \"factdb.db\"\n");
    return(0);
  }

  // 2. Check if DB is empty
  printf("Database opened successfully\n");
  sql = "SELECT * from CONFERENCE";
  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &res, NULL) != SQLITE_OK) {
    // lastmsg = sqlite3_errmsg(db);
    sqlite3_finalize(res);
    // cout << "There was an error: " << lastmsg << endl;
    loadSampleData();
  }

  // 3. Menu
  while (true) {
    choice = mainMenu();
    if (choice < 0) break;
    switch (choice) {
      case 1: listDivisions();   break;
      case 2: listTeams();       break;
      case 3: addTeam();         break;
      case 4: addPlayer();       break;
      case 5: addDivision();     break;
      case 6: addCoach();        break;
      case 7: viewTeamRoster();  break;
      case 8: viewGameInfo();    break;
      case 9: addGameInfo();     break;
      default:                   break;
    }
    cout << "\n\n";
  }
  
  // 4. Close Database
  sqlite3_close(db);
  return 0;
}

/* Definitions */

void loadSampleData() {
  string sql;
  int rc;
  string lastmsg;

  // create table CONFERENCE
  sql = "CREATE TABLE CONFERENCE (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL)";
  rc = sqlite3_exec(db, sql.c_str(), 0, (void*)data, &zerrmsg);
  if (rc) cout << zerrmsg << endl;
  sql.clear();
  // create table DIVISION
  sql = "CREATE TABLE DIVISION (id INTEGER PRIMARY KEY AUTOINCREMENT, cnf_id INTEGER NOT NULL, name TEXT NOT NULL, FOREIGN KEY (cnf_id) REFERENCES CONFERENCE (id))";
  rc = sqlite3_exec(db, sql.c_str(), 0, (void*)data, &zerrmsg);
  if (rc) cout << zerrmsg << endl;
  sql.clear();
  // create table TEAM
  sql = "CREATE TABLE TEAM (id INTEGER PRIMARY KEY AUTOINCREMENT, div_id INTEGER NOT NULL, name TEXT, mascot TEXT, city TEXT, arena TEXT, FOREIGN KEY (div_id) REFERENCES DIVISION (id))";
  rc = sqlite3_exec(db, sql.c_str(), 0, (void*)data, &zerrmsg);
  if (rc) cout << zerrmsg << endl;
  sql.clear();
  // create table PLAYER
  sql = "CREATE TABLE PLAYER (id INTEGER PRIMARY KEY AUTOINCREMENT, team_id INTEGER NOT NULL, firstname TEXT, lastname TEXT, number INTEGER, FOREIGN KEY (team_id) REFERENCES TEAM (id))";
  rc = sqlite3_exec(db, sql.c_str(), 0, (void*)data, &zerrmsg);
  if (rc) cout << zerrmsg << endl;
  sql.clear();
  // create table COACH
  sql = "CREATE TABLE COACH (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, team_id INTEGER, FOREIGN KEY (team_id) REFERENCES TEAM (id))";
  rc = sqlite3_exec(db, sql.c_str(), 0, (void*)data, &zerrmsg);
  if (rc) cout << zerrmsg << endl;
  sql.clear();
  // create table GAME
  sql = "CREATE TABLE GAME (id INTEGER PRIMARY KEY AUTOINCREMENT, date INTEGER, hometeam INTEGER, foreignteam INTEGER, finalscore TEXT, FOREIGN KEY (hometeam) REFERENCES TEAM (id), FOREIGN KEY (foreignteam) REFERENCES TEAM (id))";
  rc = sqlite3_exec(db, sql.c_str(), 0, (void*)data, &zerrmsg);
  if (rc) cout << zerrmsg << endl;
  sql.clear();

  // Add Conferences
  sql = "INSERT INTO CONFERENCE (name) VALUES ('Eastern')";
  rc = sqlite3_exec(db, sql.c_str(), 0, (void*)data, &zerrmsg);
  if (rc) cout << zerrmsg << endl;
  sql.clear();
  sql = "INSERT INTO CONFERENCE (name) VALUES ('Western')";
  rc = sqlite3_exec(db, sql.c_str(), 0, (void*)data, &zerrmsg);
  if (rc) cout << zerrmsg << endl;
  sql.clear();
}

int mainMenu() {
  int choice = 0;
  cout << "\n--- NBA Main Menu---\n";
  cout << "Choose an option (enter -1 to quit): \n";
  cout << "1. List Divisions\n";
  cout << "2. List teams in a Division\n";
  cout << "3. Add a new team\n";
  cout << "4. Add a new Player\n";
  cout << "5. Add Division\n";
  cout << "6. Add Coach\n";
  cout << "7. View roster for a team\n";
  cout << "8. View game information\n";
  cout << "9. Add game information\n";
  cout << "-1. Quit\n";
  cout << "Enter Choice: ";
  cin >> choice;
  if (choice && choice >-2 && choice < 10) {
    return choice;
  } else {
    return mainMenu();
  }
}

void listDivisions() {
  string sql = "";
  sqlite3_stmt *res;
  string lastmsg;
  int rc;
  int id;
  string name;

  sql = "SELECT * FROM DIVISION";
  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &res, NULL) != SQLITE_OK) {
    lastmsg = sqlite3_errmsg(db);
    sqlite3_finalize(res);
    cout << "There was an error: " << lastmsg << endl;
    return ;
  }
  cout << "\nID  NAME\n";
  while (sqlite3_step(res) == SQLITE_ROW) {
    id = sqlite3_column_int(res, 0);
    name = reinterpret_cast<const char*>(sqlite3_column_text(res, 2));
    cout << id << "  " << name << endl;
  }
  sqlite3_finalize(res);
  sql.clear();
}

void listTeams() {
  string sql = "";
  sqlite3_stmt *res;
  string lastmsg;
  int rc;
  int id;
  string name, mascot, city, arena;

  listDivisions();
  cout << "Enter Division ID: ";
  cin >> id;

  sql = "SELECT * FROM TEAM WHERE div_id = " + to_string(id);
  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &res, NULL) != SQLITE_OK) {
    lastmsg = sqlite3_errmsg(db);
    sqlite3_finalize(res);
    cout << "There was an error: " << lastmsg << endl;
    return ;
  }
  cout << "\nID | NAME | MASCOT | CITY | ARENA\n";
  while (sqlite3_step(res) == SQLITE_ROW) {
    id = sqlite3_column_int(res, 0);
    name = reinterpret_cast<const char*>(sqlite3_column_text(res, 2));
    mascot = reinterpret_cast<const char*>(sqlite3_column_text(res, 3));
    city = reinterpret_cast<const char*>(sqlite3_column_text(res, 4));
    arena = reinterpret_cast<const char*>(sqlite3_column_text(res, 5));
    cout << id << " | " << name << " | " << mascot << " | " << city << " | " << arena << endl;
  }
  sqlite3_finalize(res);
  sql.clear();
}

void addTeam() {
  string sql = "";
  sqlite3_stmt *res;
  string lastmsg;
  int rc;
  int id;
  string name, mascot, city, arena;

  listDivisions();
  cout << "Enter Division ID: ";
  cin >> id;
  cout << "Enter name: ";
  cin >> name;
  cout << "Enter mascot: ";
  cin >> mascot;
  cout << "Enter city: ";
  cin >> city;
  cout << "Enter arena: ";
  cin >> arena;

  sql = "INSERT INTO TEAM (div_id, name, mascot, city, arena) VALUES (";
  sql += to_string(id) + ", '" + name + "', '" + mascot + "', '" + city + "', '" + arena + "')";
  // cout << sql << endl;
  rc = sqlite3_exec(db, sql.c_str(), 0, (void*)data, &zerrmsg);
  if (rc) cout << "Team Insertion Error: " << zerrmsg << endl;
  cout << "Done..\n";
  sql.clear();
}

void addPlayer() {
  string sql = "";
  sqlite3_stmt *res;
  string lastmsg;
  int rc;
  int id, num;
  string firstname, lastname;

  cout << "Enter Team ID: ";
  cin >> id;
  cout << "Enter firstname: ";
  cin >> firstname;
  cout << "Enter lastname: ";
  cin >> lastname;
  cout << "Enter Number: ";
  cin >> num;

  sql = "INSERT INTO PLAYER (team_id, firstname, lastname, number) VALUES (";
  sql += to_string(id) + ", '" + firstname + "', '" + lastname + "', " + to_string(num) + ")";
  // cout << sql << endl;
  rc = sqlite3_exec(db, sql.c_str(), 0, (void*)data, &zerrmsg);
  if (rc) cout << "Player Insertion Error: " << zerrmsg << endl;
  cout << "Done..\n";
  sql.clear();
}

void addDivision() {
  string sql = "";
  sqlite3_stmt *res;
  string lastmsg;
  int rc;
  int id;
  string name;

  sql = "SELECT * FROM CONFERENCE";
  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &res, NULL) != SQLITE_OK) {
    lastmsg = sqlite3_errmsg(db);
    sqlite3_finalize(res);
    cout << "There was an error: " << lastmsg << endl;
    return ;
  }
  sql.clear();
  cout << "Cnferences:\n";
  cout << "ID  NAME\n";
  while (sqlite3_step(res) == SQLITE_ROW) {
    id = sqlite3_column_int(res, 0);
    name = reinterpret_cast<const char*>(sqlite3_column_text(res, 1));
    cout << id << "  " << name << endl;
  }
  sqlite3_finalize(res);
  cout << "\n";
  cout << "Enter conference_id: ";
  cin >> id;
  cout << "Enter Division Name: ";
  cin >> name;
  sql = "INSERT INTO DIVISION (cnf_id, name) VALUES (";
  sql += to_string(id) + ", '" + name + "')";
  // cout << sql << endl;
  rc = sqlite3_exec(db, sql.c_str(), 0, (void*)data, &zerrmsg);
  if (rc) cout << "Division Insertion Error: " << zerrmsg << endl;
  cout << "Done..\n";
  sql.clear();
}

void addCoach() {
  string sql = "";
  sqlite3_stmt *res;
  string lastmsg;
  int rc;
  int id;
  string name;

  cout << "Enter Team ID: ";
  cin >> id;
  cout << "Enter name: ";
  cin >> name;

  sql = "INSERT INTO COACH (name, team_id) VALUES ('";
  sql += name + "', " + to_string(id) + ")";
  // cout << sql << endl;
  rc = sqlite3_exec(db, sql.c_str(), 0, (void*)data, &zerrmsg);
  if (rc) cout << "Coach Insertion Error: " << zerrmsg << endl;
  cout << "Done..\n";
  sql.clear();
}

void viewTeamRoster() {
  string sql = "";
  sqlite3_stmt *res;
  string lastmsg;
  int rc;
  int id, num;
  string name, mascot, city, arena, firstname, lastname;

  cout << "Enter Team ID: ";
  cin >> id;
  
  sql = "SELECT * FROM TEAM WHERE id=" + to_string(id);
  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &res, NULL) != SQLITE_OK) {
    lastmsg = sqlite3_errmsg(db);
    sqlite3_finalize(res);
    cout << "There was an error: " << lastmsg << endl;
    return ;
  }
  cout << "\nID | NAME | MASCOT | CITY | ARENA\n";
  while (sqlite3_step(res) == SQLITE_ROW) {
    id = sqlite3_column_int(res, 0);
    name = reinterpret_cast<const char*>(sqlite3_column_text(res, 2));
    mascot = reinterpret_cast<const char*>(sqlite3_column_text(res, 3));
    city = reinterpret_cast<const char*>(sqlite3_column_text(res, 4));
    arena = reinterpret_cast<const char*>(sqlite3_column_text(res, 5));
    cout << id << " | " << name << " | " << mascot << " | " << city << " | " << arena << endl;
  }
  sqlite3_finalize(res);
  sql.clear();

  cout << "\nPlayers: \n";
  sql = "SELECT * FROM PLAYER WHERE team_id=" + to_string(id);
  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &res, NULL) != SQLITE_OK) {
    lastmsg = sqlite3_errmsg(db);
    sqlite3_finalize(res);
    cout << "There was an error: " << lastmsg << endl;
    return ;
  }
  cout << "\nID | FIRSTNAME | LASTNAME | NUMBER\n";
  while (sqlite3_step(res) == SQLITE_ROW) {
    id = sqlite3_column_int(res, 0);
    firstname = reinterpret_cast<const char*>(sqlite3_column_text(res, 2));
    lastname = reinterpret_cast<const char*>(sqlite3_column_text(res, 3));
    num = sqlite3_column_int(res, 4);
    cout << id << " | " << firstname << " | " << lastname << " | " << num << endl;
  }
  sqlite3_finalize(res);
  sql.clear();
}

void viewGameInfo() {
  string sql = "";
  sqlite3_stmt *res;
  string lastmsg;
  int rc;
  int id, id1, id2, date;
  string score;

  cout << "Enter home team ID: ";
  cin >> id1;
  cout << "Enter foreign team ID: ";
  cin >> id2;

  sql = "SELECT * FROM GAME WHERE hometeam=" + to_string(id1) + " AND foreignteam=" + to_string(id2);
  sql += " ORDER BY date DESC";
  if (sqlite3_prepare_v2(db, sql.c_str(), -1, &res, NULL) != SQLITE_OK) {
    lastmsg = sqlite3_errmsg(db);
    sqlite3_finalize(res);
    cout << "There was an error: " << lastmsg << endl;
    return ;
  }
  cout << "\nID | date | HOME | FOREIGN | SCORE\n";
  while (sqlite3_step(res) == SQLITE_ROW) {
    id = sqlite3_column_int(res, 0);
    date = sqlite3_column_int(res, 1);
    id1 = sqlite3_column_int(res, 2);
    id2 = sqlite3_column_int(res, 3);
    score = reinterpret_cast<const char*>(sqlite3_column_text(res, 4));
    cout << id << " | " << date << " | " << id1 << " | " << id2 << " | " << score << endl;
  }
  sqlite3_finalize(res);
  sql.clear();
}

void addGameInfo() {
  string sql = "";
  sqlite3_stmt *res;
  string lastmsg;
  int rc;
  int id1, id2, date;
  string score;

  cout << "Enter Date (Integer Timestamp): ";
  cin >> date;
  cout << "Enter home team ID: ";
  cin >> id1;
  cout << "Enter foreign team ID: ";
  cin >> id2;
  cout << "Enter final score (hometeam|foreignteam) (eg. 4|8): ";
  cin >> score;

  sql = "INSERT INTO GAME (date, hometeam, foreignteam, finalscore) VALUES (";
  sql += to_string(date) + ", " + to_string(id1) + ", " + to_string(id2) + ", '" + score + "')";
  // cout << sql << endl;
  rc = sqlite3_exec(db, sql.c_str(), 0, (void*)data, &zerrmsg);
  if (rc) cout << "Game Info Insertion Error: " << zerrmsg << endl;
  cout << "Done..\n";
  sql.clear();
}
