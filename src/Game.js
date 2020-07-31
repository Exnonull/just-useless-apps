function CheckWin (field, pos, lineSize) {
	const playerId = field.get(pos);
	let width = -1, height = -1, diag0011 = -1, diag0110 = -1;
	
	for (let x = pos.x; x >= 0; x--) {
		if (field.get({x, y:pos.y}) == playerId) width++;
		else break;
	}
	for (let x = pos.x; x < field.size.x; x++) {
		if (field.get({x, y:pos.y}) == playerId) width++;
		else break;
	}
	
	for (let y = pos.y*field.size.x; y >= 0; y -= field.size.x) {
		if (field[pos.x+y] == playerId) height++;
		else break;
	}
	for (let y = pos.y*field.size.x; y < field.length; y += field.size.x) {
		if (field[pos.x+y] == playerId) height++;
		else break;
	}
	
	for (let y = pos.y*field.size.x, x = pos.x; y < field.length && x >= 0; y += field.size.x, x--) {
		if (field[x+y] == playerId) diag0110++;
		else break;
	}
	for (let y = pos.y*field.size.x, x = pos.x; y >= 0 && x < field.size.x; y -= field.size.x, x++) {
		if (field[x+y] == playerId) diag0110++;
		else break;
	}
	
	for (let y = pos.y*field.size.x, x = pos.x; y < field.length && x < field.size.x; y += field.size.x, x++) {
		if (field[x+y] == playerId) diag0011++;
		else break;
	}
	for (let y = pos.y*field.size.x, x = pos.x; y >= 0 && x >= 0; y -= field.size.x, x--) {
		if (field[x+y] == playerId) diag0011++;
		else break;
	}
	
	return Math.max(width, height, diag0011, diag0110) >= lineSize;
}

class Game {
	size = {x: 3, y: 3}
	lineSize = 3
	players = 2
	
	winner = 0
	winCeils = []
	
	playerTurn = 1
	
	constructor (info) {
		Object.assign(this, info);
		
		this.field = new Uint8Array(this.size.x*this.size.y);
		this.field.size = this.size;
		this.field.get = function (pos) {return this[pos.x+pos.y*this.size.x]};
		this.field.set = function (pos, id) {
			this[pos.x+pos.y*this.size.x] = id
		};
	}
	
	set (pos = {x: 0, y: 0}) {
		if (this.field.get(pos)) return false;
		
		this.field.set(pos, this.playerTurn);
		if (CheckWin(this.field, pos, this.lineSize)) this.winner = this.playerTurn;
		
		this.playerTurn++; 
		if (this.playerTurn > this.players) this.playerTurn = 1;
		
		return true;
	}
	
	setId (id) {
		return this.set({
			x: id%this.field.size.x,
			y: Math.trunc(id/this.field.size.x)
		})
	}
	
	get () {
		return this.field;
	}
}

export default Game