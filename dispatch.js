'use strict';

const binding = require('./build/Release/info.node');
// const pid = Number(process.argv[2]);
const pid = process.pid;

console.log(`show process ${pid} info...`);

setInterval(() => binding.showThreadUsage(pid), 1000);
