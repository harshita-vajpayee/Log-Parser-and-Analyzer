**CrowdStrike Log Parser – Workflow Guide**

This project provides a simple CLI tool to parse CrowdStrike JSON logs and extract meaningful alert information based on selected event types.

**📥 Input File**
logs/sample.crowdstrike.log


This file should contain line-by-line JSON log entries exported from CrowdStrike.

**🚀 Running the CLI**
cd crowdstrike-parser
node src/index.js -i logs/sample.crowdstrike.log -o output/alerts.json -t ALERT,THREAT

CLI Options:

-i → Input log file

-o → Output file for filtered results

-t → Comma-separated event types to include (e.g., ALERT,THREAT)

**🔧 What the Parser Does**

Reads Input Logs
parser.js processes the log file line by line, expecting each line to be a valid JSON entry.

Extracts Important Fields
From each log entry, the parser pulls:

event_type

timestamp

hostname

alert_id

severity

Filters Based on Event Type
Only entries with event types specified in the -t flag (e.g., ALERT, THREAT) are included.

Generates Structured Output
Matching entries are written to:

output/alerts.json


The output contains clean, structured JSON representing only the filtered alerts.
