import { encode as base64_encode } from 'base-64';
import getEndPoint from '/src/utilities/getEndPoint';

const apiEndPoint : string = getEndPoint();

function loginToAPI(user : string,
                    password : string) {
  const destination = apiEndPoint;

  const authorizationHeader = 'BASIC ' + base64_encode(`${user}:${password}`);

  const headers = { 
    'Content-Type': 'application/json',
    'Authorization': authorizationHeader,
    'Connection': 'close',
  };  

  async function handleLogin() {
    const response
      = await fetch(destination, 
                    {
                      method: 'PUT',
                      headers: headers,
                      withCredentials: true,
                      crossorigin: true,
                      body: null,
                    });
    if (!response.ok) {
      const message = `An error has occurred: ${response.status}`;
      if (response.status === 403) {
        console.error(`User ${user} forbidden`);
        return null;
      }
      throw new Error(message);
    }
    const loginData = await response.json();
    return loginData;
  }

  try {
    return handleLogin();
  } catch (error) {
    console.error(`Login failed with ${error}; returning null`);
    return null;
  }

}

export default loginToAPI;
