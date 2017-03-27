import React, { Component } from 'react';
import MessageList from './MessageList';
import './Debug.css'

class Debug extends Component {
    constructor(props) {
        super(props);
    }

    render () {
        return (
            <MessageList messages={this.props.route.messages}></MessageList>
        )
    }
}

export default Debug;