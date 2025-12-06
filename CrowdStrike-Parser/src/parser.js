const fs = require('fs');
const readline = require('readline');


/**
* parseCrowdstrikeLog
* Input: path to log file (JSON lines or plain text), options
* Options: { filterEventTypes: Set, outputCSV: bool }
* Returns: Promise resolving to an array of parsed objects (or writes output)
*/


function safeGet(obj, path, fallback = null) {
return path.split('.').reduce((acc, p) => (acc && acc[p] !== undefined) ? acc[p] : fallback, obj);
}


async function parseCrowdstrikeLog(filePath, opts = {}) {
const { filterEventTypes = null, maxLines = Infinity } = opts;
const results = [];


const stream = fs.createReadStream(filePath, { encoding: 'utf8' });
const rl = readline.createInterface({ input: stream, crlfDelay: Infinity });


let lineNo = 0;
for await (const line of rl) {
if (!line.trim()) continue;
lineNo += 1;
if (lineNo > maxLines) break;


let obj = null;
try {
obj = JSON.parse(line);
} catch (e) {
// if not JSON, try to parse simple key=value style or skip
continue;
}


// adapt to common CrowdStrike fields — these may vary by log source
const eventType = safeGet(obj, 'event_type', safeGet(obj, 'type', null));
if (filterEventTypes && !filterEventTypes.has(eventType)) continue;


const timestamp = safeGet(obj, 'timestamp', safeGet(obj, 'event_timestamp', null));
const hostname = safeGet(obj, 'hostname', safeGet(obj, 'host.name', null));
const alertId = safeGet(obj, 'alert_id', safeGet(obj, 'id', safeGet(obj, 'alert.id', null)));
const severity = safeGet(obj, 'severity', null);
const raw = obj;


results.push({ eventType, timestamp, hostname, alertId, severity, raw });
}


return results;
}


module.exports = { parseCrowdstrikeLog };