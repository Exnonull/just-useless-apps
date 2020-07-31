import React from 'react'
import Ceil from './Ceil'

var key = 0;

const Area = (props) => {
	return (
		<div className='grid' style={{gridTemplateColumns:  `repeat(${props.size.x}, auto)`, gridTemplateRows:  `repeat(${props.size.y}, auto)`}}>
			{
				new Array(props.size.x*props.size.y).fill(0).map(ceil=><Ceil key={key++}/>)
			}
		</div>
	)
}

export default Area