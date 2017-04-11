import React, { Component } from 'react';
import { telemetry } from '../../App/WebSocket';
import './Deployment.css';

class Deployment extends Component {
    constructor(props) {
        super(props);

        this.state = {telemetry: telemetry};
    }

    componentDidMount() {
        this.timerID = setInterval(
            () => this.tick(),
            16
        );
    }

    componentWillUnmount() {
        clearInterval(this.timerID);
    }

    tick() {
        this.setState({
            telemetry: telemetry
        });
    }

    render() {
        const angle = this.state.telemetry;
        let deploy = null;

        if (this.state.telemetry[0] != null && 
        this.state.telemetry[0].SteeringWheelAngle > 30.0) {
            deploy = <DeployOn />
        } else {
            deploy = <DeployOff />
        }

        let steerangle = null;
        if (this.state.telemetry[0] != null) {
            steerangle = this.state.telemetry[0].SteeringWheelAngle;
        }

        return (
            <div className="root">
                <h2>Angle: {steerangle}</h2>
                {deploy}
            </div>
        )
    }
}

function DeployOn(props) {
    return <div className="barOn">On</div>
}

function DeployOff(props) {
    return <div className="barOff">Off</div>
}

export default Deployment;
