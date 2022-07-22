#include <unordered_map>
using namespace std;

enum Kind {
    KIND_STATIC,
    KIND_FIELD,
    KIND_ARG,
    KIND_VAR,
};

struct SymbolDef {
    string type;
    Kind kind;
    int index;
};

class SymbolTable {
private:
  unordered_map<string, SymbolDef> table;
  unordered_map<Kind, int> countByKind;

public:
    SymbolTable() {}

    void define(string name, string type, Kind kind) {
        if (table.contains(name)) {
            // re-define of variable
            assert(false);
        }
        int cnt = varCount(kind);
        table[name] = {type, kind, cnt};
        countByKind[kind]+=1;

        cout << "defined:" << name << "," << type << "," << kind << "," << cnt << "\n";
    }

    int varCount(Kind kind) {
        return countByKind[kind];
    }

    int kindOf(const string& name) {
        if (table.contains(name)) {
            return table[name].kind;
        } else {
            return -1;
        }
    }

    string typeOf(const string& name) {
        if (table.contains(name)) {
            return table[name].type;
        } else {
            return "";
        }
    }

    int indexOf(const string& name) {
        if (table.contains(name)) {
            return table[name].index;
        } else {
            return -1;
        }
    }

    void clear() {
        table.clear();
        countByKind.clear();
    }
};
