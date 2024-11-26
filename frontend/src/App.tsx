import React from 'react'
import reactLogo from './assets/react.svg'
import viteLogo from '/vite.svg'
import ReviewPage from '/src/components/ReviewPage';
import Box from '@chakra-ui/react';
import './App.css'

{ /* Defines the user's access level */ }
enum Access {
  Denied,
  ReadOnly,
  ReadWrite
};

{ /* Defines the user's credentials */ }
interface UserCredentials {
  name: string;
  password: string;
  jsonWebToken: string; 
  permissions: Access;
  authenticated: boolean;
};

function App() {
  const [userCredentials, setUserCredentials] = React.useState<UserCredentials>({
    name: '',
    password: '',
    jsonWebToken: null,
    permissions: Access.Denied,
    authenticated: true
  });

  if (userCredentials.authenticated) {
    return (
      <React.Fragment>
        <ReviewPage
          userCredentials={userCredentials}
        />
      </React.Fragment>
    );
  }
  else {
    return (
      <React.Fragment>
      </React.Fragment>
    );
  }
}

export default App;
