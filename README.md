# CrowdStrike Log Parser + System Log Analyzer

This repository contains **two personal projects** packaged together to demonstrate log parsing skills in **Node.js** and **C++**.

---

## Repository Structure
```
root/
├─ crowdstrike-parser/ (Node.js parser)
│ ├─ logs/
│ │ └─ sample.crowdstrike.log <-- INPUT file (CrowdStrike JSON-lines)
│ ├─ src/
│ │ ├─ parser.js <-- parsing logic
│ │ └─ index.js <-- CLI interface
│ ├─ output/ <-- OUTPUT directory (created if missing)
│ │ └─ alerts.json <-- parsed + filtered output
│ ├─ package.json <-- Node.js config
│ └─ README.md
│
├─ syslog-analyzer/ (C++ parser)
│ ├─ sample_syslog.log <-- INPUT file (syslog text)
│ ├─ src/
│ │ └─ main.cpp <-- parsing & filtering logic
│ ├─ syslog-analyzer.exe <-- compiled executable
│ ├─ output_syslog.json <-- parsed + filtered output
│ ├─ Makefile
│ └─ README.md
│
└─ README.md <-- this file
```
---

## Project Overview

### 1. CrowdStrike Log Parser (Node.js)

**Purpose:**  
Parse CrowdStrike JSON-line style logs, extract fields, filter by event type, and export results to **JSON or CSV**.

**Features:**

- Stream-based parsing for large files
- Simple filtering by event type
- Output in JSON or CSV format

**Sample Input (`logs/sample.crowdstrike.log`):**

```json
{"event_type":"ALERT","timestamp":"2025-03-01T12:34:56Z","hostname":"host-01","alert_id":"A-12345","severity":"high"}
{"event_type":"INFO","timestamp":"2025-03-01T12:35:10Z","hostname":"host-02","id":"I-22334","severity":"info"}
{"event_type":"THREAT","timestamp":"2025-03-01T12:36:00Z","hostname":"host-01","alert":{"id":"T-999"},"severity":"critical"}
```
**Output:**

crowdstrike-parser/output/alerts.json (or .csv if --csv flag is used)

**Commands to Run:**
```
cd crowdstrike-parser
npm install    # install dependencies (optional if none)
**JSON output**
node src/index.js -i logs/sample.crowdstrike.log -o output/alerts.json -t ALERT,THREAT
**CSV output**
node src/index.js -i logs/sample.crowdstrike.log -o output/alerts.csv -t ALERT,THREAT --csv
```

### 2. System Log Analyzer (C++)

**Purpose:**
Parse system logs (syslog-like text), extract date/time, host, source, PID, and message, then filter by keyword.

**Sample Input (sample_syslog.log):**
```
Mar 10 12:12:02 host-02 sshd[22345]: Failed password for invalid user admin from 10.0.0.5 port 51234 ssh2
Mar 10 12:14:09 host-03 systemd[1]: Started Session 42 of user alice.
```
**Output:**

syslog-analyzer/output_syslog.json — structured JSON of parsed and filtered log events

**Commands to Compile and Run:**

```
cd syslog-analyzer
#Compile C++ source
g++ -std=c++17 -O2 -Wall src/main.cpp -o syslog-analyzer.exe
#Run parser with optional filter
./syslog-analyzer.exe sample_syslog.log output_syslog.json --filter=Failed
```

**Explanation:**

.exe is the compiled C++ program

Parsing extracts structured fields from raw syslog text

Filtering keyword Failed keeps only matching messages

**Workflow Summary**
Project	Input File	Parsing	Filtering	Output File
CrowdStrike Parser	logs/sample.crowdstrike.log	JSON-lines → extract fields	Event types (ALERT, THREAT)	output/alerts.json
Syslog Analyzer	sample_syslog.log	Syslog text → extract datetime, host, source, PID, message	Keyword (e.g., Failed)	output_syslog.json

**Key Notes:**

Parsing = converting unstructured/semi-structured log text → structured JSON

Filtering = keeping only relevant entries based on type or keyword

Node.js parser and C++ parser are independent; they use different input formats

.exe is the compiled C++ program — required to run syslog-analyzer
