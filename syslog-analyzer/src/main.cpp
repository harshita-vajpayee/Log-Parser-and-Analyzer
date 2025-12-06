// syslog-analyzer/src/main.cpp
#include <bits/stdc++.h>
using namespace std;

struct LogEvent {
    string datetime;
    string host;
    string source;
    string pid;
    string message;
};

// trim helpers
static inline void ltrim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch){ return !isspace(ch); }));
}
static inline void rtrim(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch){ return !isspace(ch); }).base(), s.end());
}
static inline void trim(string &s) { ltrim(s); rtrim(s); }

// escape for JSON strings
string json_escape(const string &s) {
    string out;
    out.reserve(s.size() + 16);
    for (unsigned char c : s) {
        switch (c) {
            case '\"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (c < 0x20) {
                    // control char -> unicode escape
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    out += buf;
                } else {
                    out += static_cast<char>(c);
                }
        }
    }
    return out;
}

// parse a simple syslog-like format:
// "Mmm dd hh:mm:ss host source[pid]: message"
// Filtering is optional (case-insensitive substring search on message)
vector<LogEvent> parseFile(const string &filename, const string &filterKeyword) {
    ifstream in(filename);
    vector<LogEvent> out;
    if (!in.is_open()) {
        cerr << "Error: cannot open input file: " << filename << "\n";
        return out;
    }

    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;

        // Basic sanity: require at least "Mmm dd hh:mm:ss " (3 + 1 +2 +1 +8 +1 = ~16)
        if (line.size() < 16) continue;

        // Extract month (0..2), day (4..5), time (7..14)
        // Defensive: check size before substr
        string mmm, dd, time;
        try {
            mmm = line.substr(0, 3);
            dd  = line.substr(4, 2);
            time = line.substr(7, 8);
        } catch (...) {
            continue;
        }

        // remainder starts roughly at pos 16; skip spaces
        size_t idx = 15;
        while (idx < line.size() && isspace((unsigned char)line[idx])) ++idx;
        if (idx >= line.size()) continue;
        string remainder = line.substr(idx);
        trim(remainder);
        if (remainder.empty()) continue;

        // host is first token in remainder
        size_t spacePos = remainder.find(' ');
        if (spacePos == string::npos) continue;
        string host = remainder.substr(0, spacePos);
        string afterHost = remainder.substr(spacePos + 1);
        trim(afterHost);
        if (afterHost.empty()) continue;

        // split source/pid and message by first colon ':'
        string source, pid, message;
        size_t colonPos = afterHost.find(':');
        if (colonPos != string::npos) {
            string srcpart = afterHost.substr(0, colonPos);
            message = afterHost.substr(colonPos + 1);
            trim(srcpart);
            trim(message);

            // srcpart may be like "sshd[22345]" or "kernel"
            size_t lbr = srcpart.find('[');
            size_t rbr = srcpart.find(']');
            if (lbr != string::npos && rbr != string::npos && rbr > lbr) {
                source = srcpart.substr(0, lbr);
                pid = srcpart.substr(lbr + 1, rbr - lbr - 1);
                trim(source);
                trim(pid);
            } else {
                source = srcpart;
                pid.clear();
                trim(source);
            }
        } else {
            // no colon -> everything is the source, no message
            source = afterHost;
            pid.clear();
            message.clear();
            trim(source);
        }

        string datetime = mmm + " " + dd + " " + time;

        if (!filterKeyword.empty()) {
            string lowerMsg = message;
            transform(lowerMsg.begin(), lowerMsg.end(), lowerMsg.begin(), [](unsigned char c){ return static_cast<char>(tolower(c)); });
            string lk = filterKeyword;
            transform(lk.begin(), lk.end(), lk.begin(), [](unsigned char c){ return static_cast<char>(tolower(c)); });
            if (lowerMsg.find(lk) == string::npos) continue;
        }

        out.push_back({datetime, host, source, pid, message});
    }

    return out;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input-file> [output-file] [--filter=<keyword>]\n";
        return 1;
    }

    string input = argv[1];
    string output = "output_syslog.json";
    if (argc >= 3 && string(argv[2]).rfind("--", 0) != 0) {
        output = argv[2];
    }
    string filter;
    for (int i = 2; i < argc; ++i) {
        string a = argv[i];
        if (a.rfind("--filter=", 0) == 0) filter = a.substr(9);
    }

    auto events = parseFile(input, filter);

    // Write JSON array to the output path.
    // NOTE: This program does NOT create missing directories. Ensure the directory exists.
    ofstream out(output);
    if (!out.is_open()) {
        cerr << "Error: cannot open output file for writing: " << output << "\n";
        cerr << "Make sure the directory for the output file exists.\n";
        return 2;
    }

    out << "[\n";
    for (size_t i = 0; i < events.size(); ++i) {
        const auto &e = events[i];
        out << "  {\n";
        out << "    \"datetime\": \"" << json_escape(e.datetime) << "\",\n";
        out << "    \"host\": \"" << json_escape(e.host) << "\",\n";
        out << "    \"source\": \"" << json_escape(e.source) << "\",\n";
        out << "    \"pid\": \"" << json_escape(e.pid) << "\",\n";
        out << "    \"message\": \"" << json_escape(e.message) << "\"\n";
        out << "  }" << (i + 1 < events.size() ? ",\n" : "\n");
    }
    out << "]\n";

    cout << "Wrote " << events.size() << " events to " << output << "\n";
    return 0;
}
