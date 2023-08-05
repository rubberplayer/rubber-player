#include "./db.h"
#include <glibmm/miscutils.h>

std::string SelectionDB::get_database_path()
{
    std::string user_data_dir = Glib::get_user_data_dir();
    std::string full_file_path = Glib::build_filename({user_data_dir, db_filename});
    fprintf(stderr, "[db] using db [%s]\n", full_file_path.c_str());
    return full_file_path;
}
void SelectionDB::open_database()
{
    int rc;
    rc = sqlite3_open(get_database_path().c_str(), &db);
    if (rc)
    {
        fprintf(stderr, "[db] Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        opened = true;
        fprintf(stderr, "[db] Opened database successfully\n");
    }
}

static int create_tables_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        fprintf(stderr, "[db] %s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    fprintf(stderr, "[db] \n");
    return 0;
}
void SelectionDB::create_tables()
{
    if (!opened)
        return;

    int rc;
    char *zErrMsg = 0;

    auto sql = "CREATE TABLE SELECTIONS("
               "SOUND          TEXT  NOT NULL,"
               "FRAME_START    INT   NOT NULL,"
               "FRAME_END      INT   NOT NULL,"
               "LABEL          TEXT  NOT NULL"
               //               ",UNIQUE (SOUND,FRAME_START_FRAME_END)"
               ") ;";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, create_tables_callback, 0, &zErrMsg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "[db] SQL error: %d %s\n", rc, zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        tables_created = true;
        fprintf(stderr, "Table created successfully\n");
    }
}
void SelectionDB::start()
{
    open_database();
    create_tables();
}
bool SelectionDB::insert_selection(std::string path, long frame_start, long frame_end, std::string label)
{
    if (!opened)
        return false;

    int rc;
    char *zErrMsg = 0;

    auto sql = "INSERT INTO SELECTIONS (SOUND,FRAME_START,FRAME_END,LABEL) VALUES (?,?,?,?);";

    fprintf(stderr, "[db] try to insert %s, %ld, %ld, %s\n", path.c_str(), frame_start, frame_end, label.c_str());
    sqlite3_stmt *stmt;
    const char *pzTail = NULL;

    rc = sqlite3_prepare(db, sql, -1, &stmt, &pzTail);

    sqlite3_bind_text(stmt, 1, path.c_str(), path.size(), 0);
    sqlite3_bind_int64(stmt, 2, frame_start);
    sqlite3_bind_int64(stmt, 3, frame_end);
    sqlite3_bind_text(stmt, 4, label.c_str(), label.size(), 0);

    int retVal = sqlite3_step(stmt);
    if (retVal != SQLITE_DONE)
    {
        fprintf(stderr, "[db] Commit Failed! %d\n", retVal);
        return false;
    }
    rc = sqlite3_finalize(stmt);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "[db] SQL error finalize: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }
    else
    {
        fprintf(stderr, "[db] Records created successfully\n");
        return true;
    }
}

// static int callback(void *data, int argc, char **argv, char **azColName)
// {
//     int i;
//     fprintf(stderr,"[db] %s: ", (const char *)data);
//     std::vector<std::vector<std::string>> row;
//     for (i = 0; i < argc; i++)
//     {
//         std::vector<std::string> col;
//         col.push_back(azColName[i]);
//         if (argv[i])
//         {
//             col.push_back(argv[i]);
//         }
//         else
//         {
//             col.push_back("");
//         }
//         row.push_back(col);
//         fprintf(stderr,"[db] %s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//     }
//     std::vector<std::vector<std::vector<std::string>>> *rows = static_cast<std::vector<std::vector<std::vector<std::string>>> *>(data);
//     rows->push_back(row);
//     fprintf(stderr,"[db] \n");
//     return 0;
// }

std::vector<std::tuple<std::string, long, long, std::string>> *SelectionDB::load_sound_selections(std::string path)
{
    std::vector<std::tuple<std::string, long, long, std::string>> *rows = new std::vector<std::tuple<std::string, long, long, std::string>>();

    // char *zErrMsg = 0;
    int rc;
    // const char *data = "Callback function called";

    auto sql = "SELECT SOUND,FRAME_START,FRAME_END,LABEL from SELECTIONS WHERE SOUND=?";

    sqlite3_stmt *stmt;
    const char *pzTail = NULL;

    rc = sqlite3_prepare(db, sql, -1, &stmt, &pzTail);
    sqlite3_bind_text(stmt, 1, path.c_str(), path.size(), 0);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "[db] SQL error: %s\n", sqlite3_errmsg(db));
        return rows;
    }
    rc = sqlite3_step(stmt);
    /*int ncols = */ sqlite3_column_count(stmt);

    while (rc == SQLITE_ROW)
    {
        auto path = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0)));
        long frame_start = sqlite3_column_int64(stmt, 1);
        long frame_end = sqlite3_column_int64(stmt, 2);
        auto label = std::string(reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3)));
        std::tuple<std::string, long, long, std::string> row(path, frame_start, frame_end, label);
        rows->push_back(row);
        rc = sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    return rows;
}

bool SelectionDB::remove_selection(std::string path, long frame_start, long frame_end)
{
    if (!opened)
        return false;

    int rc;
    char *zErrMsg = 0;

    auto sql = "DELETE FROM SELECTIONS WHERE SOUND = ? AND FRAME_START = ? AND FRAME_END = ?;";

    fprintf(stderr, "[db] try to remove %s, %ld, %ld\n", path.c_str(), frame_start, frame_end);
    sqlite3_stmt *stmt;
    const char *pzTail = NULL;

    rc = sqlite3_prepare(db, sql, -1, &stmt, &pzTail);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "[db] Error preparing delete statement! %d\n", rc);
    }

    sqlite3_bind_text(stmt, 1, path.c_str(), path.size(), 0);
    sqlite3_bind_int64(stmt, 2, frame_start);
    sqlite3_bind_int64(stmt, 3, frame_end);

    int retVal = sqlite3_step(stmt);
    if (retVal != SQLITE_DONE)
    {
        fprintf(stderr, "[db] Commit Failed! %d\n", retVal);
        return false;
    }
    rc = sqlite3_finalize(stmt);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "[db] SQL error finalize: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }
    else
    {
        fprintf(stderr, "[db] Records deleted successfully\n");
        return true;
    }
}

void SelectionDB::close_database()
{
    if (db != NULL)
        sqlite3_close(db);
}
SelectionDB::SelectionDB()
{
    db = NULL;
    opened = false;
    tables_created = false;
}
SelectionDB::~SelectionDB()
{
    close_database();
}
