import React, { Component } from 'react';
import { Link } from 'react-router';
import SettingsForm from './SettingForm';

class Settings extends Component {
    constructor(props) {
        super(props);
        this.state = {};
    }

    render() {
        return (
            <div>
                <SettingsForm></SettingsForm>
            </div>
        )
    }
}

export default Settings;