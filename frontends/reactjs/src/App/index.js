import React from 'react';
import ReactDOM from 'react-dom';
import { Router, Route, Link } from 'react-router';
import { browserHistory } from 'react-router';
import App from './App';
import Settings from '../Views/Settings/Settings';
import Debug from '../Views/Debug/Debug';
import ERS from '../Views/ERS/ERS';
import Messages from './WebSocket';
import './index.css';

ReactDOM.render(
  <Router history={browserHistory}>
    <Route path="/" component={App} >
      <Route path="/debug" component={Debug} messages={Messages} />
      <Route path="/settings" component={Settings} />
      <Route path="/ers" component={ERS} />
    </Route>
  </Router>,
  document.getElementById('root')
);
