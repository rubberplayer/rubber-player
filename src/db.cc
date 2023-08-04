#include "./db.h"

std::string SelectionDB::get_database_path()
{
    return db_filename;
}
void SelectionDB::open_database()
{
    int rc;
    rc = sqlite3_open(get_database_path().c_str(), &db);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    }
    else
    {
        opened = true;
        fprintf(stderr, "Opened database successfully\n");
    }
}

static int create_tables_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
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
        fprintf(stderr, "SQL error: %d %s\n", rc, zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else
    {
        tables_created = true;
        fprintf(stdout, "Table created successfully\n");
    }
}
void SelectionDB::start(){
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

    printf("try to insert %s, %ld, %ld, %s\n", path.c_str(), frame_start, frame_end, label.c_str());
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
        printf("Commit Failed! %d\n", retVal);
        return false;
    }
    rc = sqlite3_finalize(stmt);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error finalize: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return false;
    }
    else
    {
        fprintf(stdout, "Records created successfully\n");
        return true;
    }
}

static int callback(void *data, int argc, char **argv, char **azColName)
{
    int i;
    fprintf(stderr, "%s: ", (const char *)data);
    std::vector<std::vector<std::string>> row;
    for (i = 0; i < argc; i++)
    {
        std::vector<std::string> col;
        col.push_back(azColName[i]);
        if (argv[i])
        {
            col.push_back(argv[i]);
        }
        else
        {
            col.push_back("");
        }
        row.push_back(col);
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    std::vector<std::vector<std::vector<std::string>>> *rows = static_cast<std::vector<std::vector<std::vector<std::string>>> *>(data);
    rows->push_back(row);
    printf("\n");
    return 0;
}

std::vector< std::tuple<std::string,long,long,std::string>> * SelectionDB::load_sound_selections(std::string path)
{
    std::vector< std::tuple<std::string,long,long,std::string>> * rows = new std::vector< std::tuple<std::string,long,long,std::string>>();
    
    char *zErrMsg = 0;
    int rc;
    const char *data = "Callback function called";

    auto sql = "SELECT SOUND,FRAME_START,FRAME_END,LABEL from SELECTIONS WHERE SOUND=?";

    sqlite3_stmt *stmt;
    const char *pzTail = NULL;

    rc = sqlite3_prepare(db, sql, -1, &stmt, &pzTail);
    sqlite3_bind_text(stmt, 1, path.c_str(), path.size(), 0);

    if (rc != SQLITE_OK)
    {
        printf( "SQL error: %s\n", sqlite3_errmsg(db));
        return rows;
    }
    rc = sqlite3_step(stmt);
    int ncols = sqlite3_column_count(stmt);

    while (rc == SQLITE_ROW)
    {
       // for (int i = 0; i < ncols; i++)
       // {
       //     printf( "'%s' ", sqlite3_column_text(stmt, i));
       // }
       // printf( "\n");
        auto path = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt,0))) ;
        long frame_start = sqlite3_column_int64(stmt,1);
        long frame_end = sqlite3_column_int64(stmt,2);
        auto label = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt,3))) ;
        std::tuple<std::string,long,long,std::string> row(path,frame_start,frame_end,label);
        rows->push_back(row);
        rc = sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
    return rows;
    //    /* Create SQL statement */
    //    auto sql = "SELECT SOUND,FRAME_START,FRAME_END,LABEL from SELECTIONS ";
    //
    //    /* Execute SQL statement */
    //    std::vector<std::vector<std::vector<std::string>>> *rows = new std::vector<std::vector<std::vector<std::string>>>();
    //    rc = sqlite3_exec(db, sql, callback, (void *)(rows), &zErrMsg);
    //    for (auto row : (*rows))
    //    {
    //        printf("~~~~~~~~~~~~~~~\n");
    //        for (auto col : (row))
    //        {
    //            printf("%s %s\n",col[0].c_str(),col[1].c_str());
    //        }
    //    }
    //    if (rc != SQLITE_OK)
    //    {
    //        fprintf(stderr, "SQL error: %s\n", zErrMsg);
    //        sqlite3_free(zErrMsg);
    //    }
    //    else
    //    {
    //        fprintf(stdout, "Operation done successfully\n");
    //    }
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
