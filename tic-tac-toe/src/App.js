import React from 'react'
import Area from './Area'
import Winner from './Winner'

const App = (props) => (
	<div>
		<Area size={props.size}/>
		<Winner />
	</div>
)

export default App