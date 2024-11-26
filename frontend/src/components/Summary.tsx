import React from 'react';
import { Box } from '@chakra-ui/react';
import { Table } from '@chakra-ui/react';
import strftime from 'strftime';
import strftimeUTC from '/src/utilities/strftimeUTC';

function AQMSEventIdentifiers( {event} ) {
  if (event) {
    var aqmsIdentifiers = ''; 
    if (Object.hasOwn(event, 'aqmsEventIdentifiers')) {
      for (var i = 0; i < event.aqmsEventIdentifiers.length; i++) {
         const thisAQMSIdentifier = event.aqmsEventIdentifiers[i];
         if (thisAQMSIdentifier.length > 0) {
           if (aqmsIdentifiers.length > 0) {
             aqmsIdentifiers = aqmsIdentifiers + ", " + thisAQMSIdentifier;
           }
           else {
             aqmsIdentifiers = thisAQMSIdentifier;
           }
         }
      }
    }
    if (aqmsIdentifiers.length > 0) {
      return (
         <Table.Row key="aqmsIdentifiers">
          <Table.Cell>AQMS Identifiers</Table.Cell>
          <Table.Cell>{aqmsIdentifiers}</Table.Cell>
        </Table.Row>
      );
    }
  }
  return null;
}

function Summary( {event, height} ) {

  //var AQMSIdentifiers = getAQMSEventIdentifiers(event);

  if (event) {
    return (
      <React.Fragment>
        <Box w='100%' h='100%' bg='white'>
          <Table.Root size="lg" variant="outline" striped>
            <Table.Body>
              <Table.Row key="identifier">
                <Table.Cell>Identifier</Table.Cell>
                <Table.Cell>{event.identifier}</Table.Cell>
              </Table.Row>
              <AQMSEventIdentifiers event={event}/>
              <Table.Row key="time">
                <Table.Cell>Origin Time</Table.Cell>
                <Table.Cell>{strftimeUTC('%Y-%m-%dT%H:%M:%S.%fZ', event.origin.time)}</Table.Cell>
              </Table.Row>
              <Table.Row key="latitude">
                <Table.Cell>Latitude</Table.Cell>
                <Table.Cell>{event.origin.latitude.toFixed(5)}</Table.Cell>
              </Table.Row>
              <Table.Row key="longitude">
                <Table.Cell>Longitude</Table.Cell>
                <Table.Cell>{event.origin.longitude.toFixed(5)}</Table.Cell>
              </Table.Row>
              <Table.Row key="depth">
                <Table.Cell>Depth</Table.Cell>
                <Table.Cell>{event.origin.depth.toFixed(1)}</Table.Cell>
              </Table.Row>
              <Table.Row key="nPhases">
                <Table.Cell>Number of Phases</Table.Cell>
                <Table.Cell>{event.origin.arrivals.length}</Table.Cell> 
              </Table.Row>
              <Table.Row key="source">
                <Table.Cell>Source</Table.Cell>
                <Table.Cell>Machine Learning</Table.Cell>
              </Table.Row>
            </Table.Body>
          </Table.Root>
        </Box>
      </React.Fragment>
    );
  }
  else {
    return (
      <React.Fragment>
        <h2>No event selected</h2>
      </React.Fragment>
    );
  }
}

export default Summary;
