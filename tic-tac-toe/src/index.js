import React from 'react'
import ReactDOM from 'react-dom'
import App from './App'
import Game from './Game'

const size = {x:3, y:3};
const players = 2;
const lineSize = 3;

ReactDOM.render(<App size={size}/>, document.getElementById('wrapper'));

function startTheGame(){
	const kek = new Game({size,lineSize,players});//3x3, 3 strike, 2 players
	kek.dom = document.getElementsByClassName('grid-item');
	Array.from(kek.dom).forEach((ge, id) => {
		ge.onclick = e => {
			if (kek.winner) return false;
			
			const status = kek.setId(id);
			if (kek.winner) {
				console.log(kek.winner);
				if (kek.winner < 0) kek.winner = 0;
				document.getElementById('winner').style.display='block';
				document.getElementById('winnerName').innerHTML = ['nobody','red','blue','green','yellow'][kek.winner];
				document.getElementById('winner').onclick = e => {
					document.onclick=null;
					document.getElementById('winner').style.display='none';
					document.getElementById('winnerName').innerHTML = '';
					Array.from(kek.dom).forEach(ge => ge.style.backgroundColor = 'gray');
					startTheGame();
					e.preventDefault();
				}
			}
			if (status) ge.style.backgroundColor = ['gray', 'red', 'blue', 'green', 'yellow'][kek.field[id]];
			e.preventDefault();
		};
	})
}
startTheGame();