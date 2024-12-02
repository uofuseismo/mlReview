import { encode as base64_encode } from 'base-64';
import getEndPoint from '/src/utilities/getEndPoint';

const apiEndPoint : string = getEndPoint();

function fetchAsyncEventsFromAPI(user : string,
                                 password : string) {
  const destination = apiEndPoint;

  const authorizationHeader = 'BASIC ' + base64_encode(`${user}:${password}`);

  const headers = { 
    'Content-Type': 'application/json',
    'Authorization': authorizationHeader,
    'Connection': 'close',
  };  

  const requestData = {
    resource: 'catalog'
  };

  console.debug(`Fetching events from ${destination}`);

  async function handleGetData() {
    const response
      = await fetch(destination, 
                    {
                      method: 'PUT',
                      headers: headers,
                      withCredentials: true,
                      crossorigin: true,
                      body: JSON.stringify(requestData),
                    });
    if (!response.ok) {
      const message = `An error has occurred: ${response.status}`;
      throw new Error(message);
    }
    const eventData = await response.json();
    return eventData;
  }

  try {
    return handleGetData();
  } catch (error) {
    console.log(error);
    return null;
  }
};

export default fetchAsyncEventsFromAPI;
