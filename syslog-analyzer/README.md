**Syslog Analyzer – Workflow Guide (C++)**

This tool provides a lightweight way to parse traditional Linux-style syslog files and convert them into clean, structured JSON, with optional keyword-based filtering.

**📥 Input File**
sample_syslog.log


The log should follow standard syslog formatting with timestamps, hostnames, process info, and messages.

**🛠️ Compile the Project**

Use g++ with C++17 support:

cd syslog-analyzer
g++ -std=c++17 -O2 -Wall src/main.cpp -o syslog-analyzer.exe


This command builds the executable:

syslog-analyzer.exe

🚀 Run the Parser
./syslog-analyzer.exe sample_syslog.log output_syslog.json --filter=Failed

Arguments:

sample_syslog.log → Input syslog file

output_syslog.json → Output JSON file

--filter=Failed → Optional keyword; only lines containing this term are included

**🔧 What the Analyzer Does**

Reads Each Log Line
The executable processes the syslog file line by line.

Parses Key Fields
Each entry is broken down into:

datetime

host

source (process name)

pid

message

Filtering
Only log lines containing the specified keyword (e.g., Failed) are selected.
You can replace this keyword with any string you want the parser to search for.

**JSON Output**
Matched entries are written to:

output_syslog.json


The structure is clean, consistent JSON ready for analysis or ingestion into other tools.
