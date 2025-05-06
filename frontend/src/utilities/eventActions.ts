import { encode as base64_encode } from 'base-64';
import getEndPoint from '/src/utilities/getEndPoint';

const apiEndPoint : string = getEndPoint();

function createBearerHeader(jwt : string) {
  const authorizationHeader = `BEARER ${jwt}`;

  const headers = { 
    'Content-Type': 'application/json',
    'Authorization': authorizationHeader,
    'Connection': 'close',
  };  
}

function asyncAcceptEventToAWS(jwt : string,
                               eventIdentifier : string) {
  if (eventIdentifier == '') {
    throw 'Event identifier is empty';
  }

  const destination = apiEndPoint;
  const headers = createBearerHeader(jwt);
  const requestData = { 
    resource: 'actions/acceptToAWS',
    identifier: eventIdentifier
  };  

  console.debug(`Submitting acceptEventToAWS action for ${eventIdentifier} to ${destination}`);

  async function handleAction() {
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
    const responsePayload = await response.json();
    return responsePayload;
  }

  try {
    return handleAction();
  }
  catch (error) {
    console.warn(error);
    return null;
  }
}

function asyncAcceptEventToAQMS(jwt : string,
                                eventIdentifier : string) {
  if (eventIdentifier == '') {
    throw 'Event identifier is empty';
  }

  const destination = apiEndPoint;
  const headers = createBearerHeader(jwt);
  const requestData = { 
    resource: 'actions/acceptToAQMS',
    identifier: eventIdentifier
  };  

  console.debug(`Submitting acceptEventToAQMS action for ${eventIdentifier} to ${destination}`);

  async function handleAction() {
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
    const responsePayload = await response.json();
    return responsePayload;
  }

  try {
    return handleAction();
  }
  catch (error) {
    console.warn(error);
    return null;
  }
}

function asyncRejectEvent(jwt : string,
                          eventIdentifier : string) {
  if (eventIdentifier == '') {
    throw 'Event identifier is empty';
  }

  const destination = apiEndPoint;
  const headers = createBearerHeader(jwt);
  const requestData = { 
    resource: 'actions/rejectEvent',
    identifier: eventIdentifier
  };

  console.debug(`Submitting acceptEventToAWS action for ${eventIdentifier} to ${destination}`);

  async function handleAction() {
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
    const responsePayload = await response.json();
    return responsePayload;
  }

  try {
    return handleAction();
  }
  catch (error) {
    console.warn(error);
    return null;
  }
}

export { asyncAcceptEventToAWS, asyncAcceptEventToAQMS, asyncRejectEvent };


