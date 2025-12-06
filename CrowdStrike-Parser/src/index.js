#!/usr/bin/env node
const fs= require('fs');
const path= require('path');
const {parseCrowdstrikeLog} = require('./parser');

function printHelp(){
    console.log(` Usage: node src/index.js -i <input-file> [-o <output-file>] [-t <comma-separated-event-types>] [--csv]

        Options:
        -i, --input Input log file path (required)
        -o, --output Output file path (default: output/results.json)
        -t, --types Comma-separated list of event types to keep (e.g. ALERT,THREAT)
        --csv Output CSV instead of JSON
        --maxlines Maximum lines to parse (useful for testing)
        -h, --help Show this help
        `);
    }


async function main() {
const argv = process.argv.slice(2);
if (argv.length === 0 || argv.includes('-h') || argv.includes('--help')) {
printHelp();
process.exit(0);
}


const getArg = (flags) => {
for (let i = 0; i < argv.length; i++) {
if (flags.includes(argv[i])) return argv[i+1];
}
return null;
};


const input = getArg(['-i', '--input']);
const output = getArg(['-o', '--output']) || 'output/results.json';
const typesArg = getArg(['-t', '--types']);
const csvFlag = argv.includes('--csv');
const maxLinesArg = getArg(['--maxlines']);
const maxLines = maxLinesArg ? parseInt(maxLinesArg, 10) : Infinity;


if (!input) {
console.error('Error: input file is required');
printHelp();
process.exit(1);
}


const filterEventTypes = typesArg ? new Set(typesArg.split(',').map(s => s.trim())) : null;


try {
const parsed = await parseCrowdstrikeLog(input, { filterEventTypes, maxLines });


// Prepare output dir
const outdir = path.dirname(output);
if (!fs.existsSync(outdir)) fs.mkdirSync(outdir, { recursive: true });


if (csvFlag) {
const header = ['eventType', 'timestamp', 'hostname', 'alertId', 'severity'];
const lines = [header.join(',')];
for (const r of parsed) {
const row = header.map(h => {
const v = r[h] === null || r[h] === undefined ? '' : String(r[h]);
return '"' + v.replace(/"/g, '""') + '"';
}).join(',');
lines.push(row);
}
fs.writeFileSync(output, lines.join('\n'));
} else {
fs.writeFileSync(output, JSON.stringify(parsed, null, 2));
}


console.log(`Parsed ${parsed.length} matching events. Output written to ${output}`);
} catch (e) {
console.error('Failed to parse:', e);
process.exit(2);
}
}


main();