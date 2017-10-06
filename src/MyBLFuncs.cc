// AK20100820 Various functions from G4beamline v2.03

#include "MyBLFuncs.hh"

std::map<G4String,G4String> *MyBLFuncs::paramMap;
//std::map<G4String,G4String> *BLParam::paramHelpText;

MyBLFuncs BLFuncs;                                    // the global BLFuncs object

void MyBLFuncs::init() {
    if (!paramMap) paramMap = new std::map<G4String,G4String>;
    //if (!paramHelpText) paramHelpText = new std::map<G4String,G4String>;
}


// AK20100820 parseArgs is from BLCommand.cc
int MyBLFuncs::parseArgs(const G4String &line, MyBLArgumentVector &argv,
MyBLArgumentMap &namedArgs) {
    G4String::size_type place = 0;
    while(place < line.size()) {
        TokenType type;
        G4String arg = nextToken(line,place,type), val;
        switch(type) {
            case NONE:
                break;
            case ARGNAME:
                val = nextToken(line,place,type);
                if(type != ARGVALUE) {
                    G4cerr << "Syntax error parsing arguments" << G4endl;
                    return -1;
                }
                namedArgs[arg] = expand(val);
                break;
            case ARGVALUE:
                argv.push_back(expand(arg));
                break;
        }
    }
    return 0;
}


// AK20100820 nextToken is from BLCommand.cc
G4String MyBLFuncs::nextToken(const G4String& line, G4String::size_type& place,
TokenType& type) {
    // (add +- for particlecolor pi+=1,1,1)
    static const char namechars[] = ",+-ABCDEFGHIJKLMNOPQRSTUVWXYZ_"
        "abcdefghijklmnopqrstuvwxyz0123456789";
    G4String::size_type i;

    // check if previous token was ARGNAME
    if(line[place] == '=') {
        ++place;
        goto value;
    }

    // skip initial whitespace
    while(place < line.size() && isspace(line[place])) ++place;

    // check for ARGNAME
    if(isalnum(line[place]) || line[place] == '_') {
        i = line.find_first_not_of(namechars,place);
        if(i > place && i < line.size() && line[i] == '=' &&
        line[i+1] != '=') {
            G4String retval = line.substr(place,i-place);
            place = i;
            type = ARGNAME;
            return retval;
        }
    }
    value:
    if(line[place] == '"') {
        ++place;
        i = line.find('"',place);
        if(i <line.size()) {
            G4String retval = line.substr(place,i-place);
            place = i + 1;
            type = ARGVALUE;
            return retval;
        }
    }
    else if(line[place] == '\'') {
        ++place;
        i = line.find('\'',place);
        if(i <line.size()) {
            G4String retval = line.substr(place,i-place);
            place = i + 1;
            type = ARGVALUE;
            return retval;
        }
    }

    if(place >= line.size()) {
        type = NONE;
        return "";
    }

    // find next whitespace
    G4String::size_type start = place;
    while(place < line.size() && !isspace(line[place])) ++place;

    type = ARGVALUE;

    return line.substr(start,place-start);
}


// AK20100820 expand is from BLParam.cc
G4String MyBLFuncs::expand(G4String str) {
    static G4String nameChars("ABCDEFGHIJKLMNOPQRSTUVWXYZ_"
        "abcdefghijklmnopqrstuvwxyz0123456789");
    G4String out;

    G4String::size_type place=0;
    while(place < str.size()) {
        G4String::size_type i=str.find('$',place);
        if(i == str.npos) {
            out += str.substr(place);
            break;
        }
        out += str.substr(place,i-place);
        place = i + 1;
        // leave $1 - $9 and $# alone (for define command)
        if(isdigit(str[place]) || str[place] == '#') {
            out += "$";
            continue;
        }
        // replace $$ with $ (delayed evaluation of $param)
        if(str[place] == '$') {
            out += "$";
            ++place;
            continue;
        }
        G4String::size_type j=str.find_first_not_of(nameChars,place);
        if(j == str.npos) j = str.size();
        G4String name=str.substr(place,j-place);
        if(j == place || isdigit(str[place])) {
            G4cerr << "MyBLFuncs::expand ERROR: Invalid parameter name "
                << name << G4endl;
        }
        else {
            out += getString(name);
        }
        place = j;
    }

    return out;
}


// AK20100820 getString is from BLParam.cc
G4String MyBLFuncs::getString(G4String name) {
    init();
    if((*paramMap).count(name) == 0) {
        // define it from the environment, if possible
        char *p = getenv(name.c_str());
        if(p) {
            setParam(name,p);
        }
        else {
            G4cerr << "MyBLFuncs::getString ERROR: Unknown parameter "
                << name << G4endl;
        }
    }
    return (*paramMap)[name];                         // "" if not found
}


// AK20100820 setParam is from BLParam.cc
void MyBLFuncs::setParam(G4String name, G4String value) {
    init();
    (*paramMap)[name] = value;
}


// AK20100820 setParam is from BLParam.cc
void MyBLFuncs::setParam(G4String name, G4double value) {
    char tmp[32];
    sprintf(tmp,"%g",value);
    setParam(name,tmp);
}


// AK20100820 setParam is from BLParam.cc
void MyBLFuncs::setParam(G4String name, G4int value) {
    char tmp[32];
    sprintf(tmp,"%d",value);
    setParam(name,tmp);
}
