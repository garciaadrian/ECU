import React, { Component } from 'react';
import {iracingConnection} from '../../App/WebSocket';
import './Status.css';

class Status extends Component {
    constructor(props) {
        super(props);
        this.state = {connection: iracingConnection};
    }

    componentDidMount() {
        this.timerID = setInterval(
            () => this.tick(),
            2500
        );
    }

    componentWillUnmount() {
        clearInterval(this.timerID);
    }

    tick() {
        this.setState({
            connection: iracingConnection
        });
    }

    render() {
        const connection = this.state.connection;
        let button = null;

        if (connection.status == 1) {
            button = <Connected />;
        } else {
            button = <Disconnected />;
        }

        return (
            <div>
                {button}
            </div>
        )
    }
}

function Connected(props) {
    return <li className="statusHeader">Status: <span className="statusConnected">Connected</span></li>;
}

function Disconnected(props) {
    return <li className="statusHeader">Status: <span className="statusDisconnected">Disconnected</span></li>;
}

export default Status;