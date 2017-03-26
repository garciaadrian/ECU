import React, { Component } from 'react';

class SettingsForm extends Component {
    constructor(props) {
        super(props);

        this.state = {value: ''};
        this.handleChange = this.handleChange.bind(this);
        this.handleSubmit = this.handleSubmit.bind(this);
    }

    handleChange(event) {
        this.setState({value: event.target.value});
        console.log("form value has changed");
    }

    handleSubmit(event) {
        alert("Settings were updated: " + this.state.value);
        event.preventDefault();
    }

    render() {
        return (
            <form onSubmit={this.handleSubmit}>
                <label>
                    Driver Name:
                    <input type="text" value={this.state.value} onChange={this.handleChange} />
                </label>
                <input type="submit" value="SAVE" />
            </form>
        )
    }
}

export default SettingsForm;