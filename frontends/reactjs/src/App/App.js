import React, { Component } from 'react';
import NavBar from '../Layout/NavBar/NavBar'
import './App.css';

class App extends Component {
  constructor(props) {
    super(props);
    this.state = {};
  }

  render() {
    return (
      <div className="App">
        <NavBar></NavBar>
        {this.props.children}
      </div>
    );
  }
}

export default App;
