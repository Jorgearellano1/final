#include <iostream>
#include <climits>
#include <string>
#include <vector>
#include "httplib.h"

using namespace std;

template <typename TK, typename TV>
struct KeyValuePair {
    TK key;
    TV value;

    KeyValuePair() : key(TK()), value(TV()) {}
    KeyValuePair(TK k, TV v) : key(k), value(v) {}
};

template <typename TK, typename TV>
class CuckooHashing
{
private:
    static const int ver = 2;
    KeyValuePair<TK, TV> **hashtable;
    int *pos;
    int size;
    int MAXLOOP;
public:
    CuckooHashing(int initial_size = 15)
    {
        size = initial_size;
        MAXLOOP = size;
        hashtable = new KeyValuePair<TK, TV> *[ver];
        pos = new int[ver];
        for (int i = 0; i < ver; ++i)
            hashtable[i] = new KeyValuePair<TK, TV>[size];
        initTable();
    }

    ~CuckooHashing()
    {
        for (int i = 0; i < ver; ++i)
            delete[] hashtable[i];
        delete[] hashtable;
        delete[] pos;
    }

    void initTable()
    {
        for (int j = 0; j < size; j++)
            for (int i = 0; i < ver; i++)
                hashtable[i][j] = KeyValuePair<TK, TV>();
    }

    int hash(int function, int key)
    {
        switch (function)
        {
            case 1:
                return (key % size + size) % size;
            case 2:
                return ((key / size) % size + size) % size;
        }
        return 0;
    }

    void place(const KeyValuePair<TK, TV> &key, int tableID, int cnt,string& result)
    {
        if (cnt == MAXLOOP)
        {
            result += "REHASH\n";
            rehash(result);
            place(key, 0, 0,result);
            return;
        }

        for (int i = 0; i < ver; i++)
        {
            pos[i] = hash(i + 1, key.key);
            if (hashtable[i][pos[i]].key == key.key)
                return;
        }

        if (hashtable[tableID][pos[tableID]].key != TK())
        {
            KeyValuePair<TK, TV> displaced_element = hashtable[tableID][pos[tableID]];
            result += "Collision ! : Moviendo ";
            result += to_string(displaced_element.key);
            result += " hacia la tabla " ;
            result += to_string((tableID + 1) % ver);
            result += " en la pos " ;
            result += to_string(pos[tableID]);
            result += "\n" ;



            hashtable[tableID][pos[tableID]] = key;
            place(displaced_element, (tableID + 1) % ver, cnt + 1,result);
        }
        else
        {            result += "<";

            result += to_string(key.key);
            result += ":";
            result += (key.value);
            result += ">";

            result += " Tabla : ";
            result += to_string(tableID);
            result += " , Posición : ";
            result += to_string(pos[tableID]);


            hashtable[tableID][pos[tableID]] = key;
        }
    }

    bool find(const TK&key)
    {
        for (int i = 0; i < ver; i++)
        {
            int position = hash(i + 1, key);
            if (hashtable[i][position].key == key)
            {
                return true;
            }
        }

        return false;
    }

    void rehash(string& result)
    {
        KeyValuePair<TK, TV> **old_table = hashtable;
        int old_size = size;

        size *= 2;
        MAXLOOP = size;

        hashtable = new KeyValuePair<TK, TV> *[ver];
        for (int i = 0; i < ver; ++i)
            hashtable[i] = new KeyValuePair<TK, TV>[size];
        initTable();

        for (int i = 0; i < ver; ++i)
            for (int j = 0; j < old_size; ++j)
                if (old_table[i][j].key != TK())
                    place(old_table[i][j], 0, 0,result);



        for (int i = 0; i < ver; ++i)
            delete[] old_table[i];
        delete[] old_table;
    }


    vector<string> printTable()
    {
        vector<string> tablas = {"", ""};

        for (int i = 0; i < ver; i++)
        {
            for (int j = 0; j < size; j++)
            {
                if (hashtable[i][j].key == TK())
                {
                    tablas[i] += "- ";
                }
                else
                {
                    tablas[i] += to_string(hashtable[i][j].key);
                    tablas[i] += ":";
                    tablas[i] += (hashtable[i][j].value);
                    tablas[i] += " ";
                }
            }
        }


        return tablas;
    }


    string insert(const KeyValuePair<TK, TV> &key)
    {
        string  result =  "Posibles posiciones para el hash:\n" ;
        for (int i = 0; i < ver; i++)
        {
            pos[i] = hash(i + 1, key.key);
            result  += "Tabla";
            result  += to_string(i);
            result += ": ";
            result  += to_string(pos[i]);
            result   += "\n";
        }

        place(key, 0, 0,result);

        return result;
    }

    bool remove(const TK &key)
    {
        for (int i = 0; i < ver; i++)
        {
            int position = hash(i + 1, key);
            if (hashtable[i][position].key == key)
            {
                hashtable[i][position] = KeyValuePair<TK, TV>();
                return true;
            }
        }

        return false;
    }
};





int main() {
    using namespace httplib;

    Server svr;
    CuckooHashing<int,string> cuckooHash;

    svr.Get("/insert", [&](const httplib::Request &req, httplib::Response &res) {
        res.set_header("Access-Control-Allow-Origin", "http://localhost:3000");
        int key = std::stoi(req.get_param_value("key"));
        std::string value = req.get_param_value("value");
        auto result = cuckooHash.insert(KeyValuePair<int, string>(key, value));
        res.set_content(result, "text/plain");
    });

    svr.Get("/search", [&](const httplib::Request &req, httplib::Response &res) {
        res.set_header("Access-Control-Allow-Origin", "http://localhost:3000");

        int key = std::stoi(req.get_param_value("key"));
        bool found = cuckooHash.find(key);
        std::string response = found ? "Elemento encontrado" : "Elemento no encontrado";
        res.set_content(response, "text/plain");
    });

    svr.Get("/delete", [&](const httplib::Request &req, httplib::Response &res) {
        res.set_header("Access-Control-Allow-Origin", "http://localhost:3000");

        int key = std::stoi(req.get_param_value("key"));
        bool removed = cuckooHash.remove(key);
        std::string response = removed ? "Elemento eliminado" : "Elemento no encontrado para eliminar";
        res.set_content(response, "text/plain");
    });

    svr.Get("/print", [&](const httplib::Request &req, httplib::Response &res) {
        res.set_header("Access-Control-Allow-Origin", "http://localhost:3000");

        auto tablas = cuckooHash.printTable();
        res.set_content(tablas[0] + "\n" + tablas[1], "text/plain");
    });

    svr.Options(R"(.*)", [](const Request&, Response& res) {
        res.set_header("Access-Control-Allow-Origin", "http://localhost:3000");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");
        res.status = 204; // No Content
    });
    svr.listen("localhost", 8080);

    return 0;
}