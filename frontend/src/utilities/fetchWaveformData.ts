import { encode as base64_encode } from 'base-64';
import getEndPoint from '/src/utilities/getEndPoint';

const apiEndPoint : string = getEndPoint();

function fetchAsyncWaveformsFromAPI(identifier : string,
                                    user : string,
                                    password : string) {
  const destination = apiEndPoint; //`${apiEndPoint}?application=eventmaphash`;

  const authorizationHeader = 'BASIC ' + base64_encode(`${user}:${password}`);

  const headers = { 
    'Content-Type': 'application/json',
    'Authorization': authorizationHeader,
    'Connection': 'close',
    //'Access-Control-Allow-Origin': '*' ,
    //'Access-Control-Allow-Methods': 'GET,HEAD,OPTIONS,POST,PUT',
    //'Access-Control-Allow-Headers': 'Access-Control-Allow-Headers, Origin, Accept, X-Requested-With, Content-Type, Access-Control-Request-Method, Access-Control-Request-Headers, Authorization',
  };  

  const requestData = {
    resource: 'waveforms',
    identifier: Number(identifier)
  };

console.log(destination);
console.log(headers);
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
    const waveformData = await response.json();
console.log(waveformData);
    return waveformData;
  }

  try {
    return handleGetData();
  } catch (error) {
    console.log(error);
    return null;
  }
};

export default fetchAsyncWaveformsFromAPI;
