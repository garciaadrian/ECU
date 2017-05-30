import React, { Component, PropTypes } from 'react';
import Deployment from './Deployment';

class ERS extends Component {
    constructor(props) {
        super(props);
    }

    render() {
        return (
            <div>
                <Deployment />
            </div>
        )
    }
}

export default ERS;