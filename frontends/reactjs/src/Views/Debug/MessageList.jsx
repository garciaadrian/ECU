import React, { Component } from 'react';
import './MessageList.css'

class MessageList extends Component {
    constructor(props) {
        super(props);
        this.state = {messages: props.messages};
    }

    componentDidMount() {
        this.timerID = setInterval(
            () => this.tick(),
            1000
        );
    }

    componentWillUnmount() {
        clearInterval(this.timerID);
    }

    tick() {
        this.setState({
            messages: this.props.messages
        });
    }

    render() {
        var items = [];

        for (var i = 0; i < this.props.messages.length; i++) {
            items.push(<li key={i} className="message">Message: {i} - {this.props.messages[i]}</li>);
        }

        return (
            <div>
                <h3>Messages:</h3>
                <ul className="messageList">
                    {items}
                </ul>
            </div>
        )
    }
}

export default MessageList;