import React from 'react'
import reactLogo from './assets/react.svg'
import viteLogo from '/vite.svg'
import Login from '/src/components/Login';
import ReviewPage from '/src/components/ReviewPage';
import loginToAPI from '/src/utilities/loginToAPI';
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
    authenticated: false
  });

  const handleLogin = ( userValue, passwordValue ) => {
    loginToAPI(userValue,
               passwordValue).then(result => {
      if (result)
      {
        if (result.statusCode == 200) {
          console.log('Successfully logged into API');
          const newUserCredentials
            = {...userCredentials,
               name: userValue,
               password: passwordValue,
               authenticated: true,
               jsonWebToken: result.data.jsonWebToken,
               permissions: Access.ReadWrite};
          setUserCredentials(newUserCredentials); 
        }
        else {
          console.error('Invalid credentials');
        }
      }
    })
    .catch(error => {
      console.error(error);
    });
    //var loggedIn = loginToAPI( userValue, passwordValue, handleUserCredentials );
    //if (loggedIn) {
    //  return (
    //    <React.Fragment>
    //    </React.Fragment>
    //  );
    //}
    //else {
    //  return (
    //    <React.Fragment>
    //      <Alert status='error'>
    //        <AlertIcon />
    //        <AlertTitle>Login failed!</AlertTitle>
    //        <AlertDescription>Could not validate your credentials.</AlertDescription>
    //      </Alert>
    //    </React.Fragment>
    //  );
    //}
  }


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
        <Login
         initialUser={userCredentials.name}
         initialPassword={userCredentials.password}
         onHandleLogin={handleLogin} />
      </React.Fragment>
    );
  }
}

export default App;
