#include <sqlite3.h>
#include <vector>
#include <string>

class SelectionDB
{
private:
    std::string db_filename = "rubber-player-selections.db3";
    sqlite3 *db;
    bool opened;
    bool tables_created;
    std::string get_database_path();
public:
    SelectionDB();
    ~SelectionDB();
    void open_database();
    void create_tables();
    bool insert_selection(std::string path,long frame_start, long frame_end, std::string label );
    void load_selections();
    void close_database();
};

// https://stackoverflow.com/questions/1601151/how-do-i-check-in-sqlite-whether-a-table-exists
// https://stackoverflow.com/questions/400257/how-can-i-pass-a-class-member-function-as-a-callback