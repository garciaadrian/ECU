import React, { Component } from 'react';
import { Link } from 'react-router';
import './NavBar.css'

class NavBar extends Component {
    constructor(props) {
        super(props);
        this.state = {isLoggedIn: false};

        this.handleLogin = this.handleLogin.bind(this);
    }
    handleLogin() {
        if (!this.state.isLoggedIn) {
            console.log("Logged In!");
        } else {
            console.log("Logged out!");
        }
        this.setState(prevState => ({
            isLoggedIn: !prevState.isLoggedIn
        }));
    }
    render() {
        const isLoggedIn = this.state.isLoggedIn;
        let button = null;

        if (isLoggedIn) {
            button = <LogoutButton onClick={this.handleLogin} />
        } else {
            button = <LoginButton onClick={this.handleLogin} />
        }

        return (
            <div>
                 <ul className="menu">
                    <li className="menuItem"><Link to="/debug">Debug</Link></li>
                    <li className="menuItem"><Link to="/settings">Settings</Link></li>
                </ul>
            </div>
        )
    }
}

function UserGreeting(props) {
    return <h1>Welcome, User</h1>;
}

function GuestGreeting(props) {
    return <h1>Please sign in</h1>;
}

function Greeting(props) {
    const isLoggedIn = props.isLoggedIn;
    if (isLoggedIn) {
        return <UserGreeting />;
    }
    return <GuestGreeting />;
}

function LoginButton(props) {
    return (
        <button onClick={props.onClick}>
            Login
        </button>
    )
}

function LogoutButton(props) {
    return (
        <button onClick={props.onClick}>
            Logout
        </button>
    )
}

export default NavBar;