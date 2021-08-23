const fs = require('fs');
const path = require('path');

let headerTemplateHeader = `#ifndef sounds_h
#define sounds_h

/** 
 * This file is generated
 * Do not edit 
 * 
 * To regenerate, run \`cd ../sounds && node ./prepare.js\`
 **/

enum Sounds : byte {
`;

let headerTemplateFooter = `
};

#endif`;

let srcDir = './src';
let destDir = '../data';

let files = fs.readdirSync(srcDir);

let numbers = files.filter(f => f.startsWith('Number'));
let letters = files.filter(f => f.startsWith('Nato'));
let rest = files.filter(f => numbers.indexOf(f) == -1 && letters.indexOf(f) == -1);

function copyToDest(file, num)
{
	fs.copyFileSync(path.resolve(path.join(srcDir, file)), path.join(destDir, num + '.mp3'))
}

// Process the numbers, they are all offset at 100
let linesNumbers = numbers.map(file => {
	let num = parseInt(file.substr("Number".length), 10);
	copyToDest(file, num);
	return '	Number' + num + ' = ' + (100 + num);
});


// Process the letters, they are all offset at 200
let linesLetters = letters.map(file => {
	let num = file.charCodeAt(4) - 65;
	copyToDest(file, num);
	return '	' + file.substr(0, file.length - 4) + ' = ' + (200 + num);
});

// Process the rest of the lines
let linesRest = rest.map((file, index) => {
	let num = index + 1;
	let name = file.substr(0, file.length - 4);
	let cpp = '	' + name + ' = ' + num;
	copyToDest(file, num);
	return cpp;
});

// build output
let lines = linesRest.concat(linesNumbers).concat(linesLetters);
let output = headerTemplateHeader + lines.join(',\n') + headerTemplateFooter;

console.log(output);

fs.writeFileSync('../../common/sounds.h', output);