import React from 'react';
import { Box, HStack } from '@chakra-ui/react';
import { Table } from '@chakra-ui/react';
import EventStationMap from '/src/components/EventStationMap';
import strftimeUTC from '/src/utilities/strftimeUTC';

function LocationReview( {event, stations, height} ) {

  var stationsToPlot = [];
  var arrivals = [];
  if (event) {
    stationsToPlot = []; 
    arrivals = event.origin.arrivals;
    for (const arrival of arrivals) {
      const foundStation = stations.find( function(station) {
        return arrival.network === station.network &&
               arrival.station === station.name;
      }); 
      if (foundStation != undefined) {
        if (!stationsToPlot.includes(foundStation)) {
          stationsToPlot.push(foundStation);
        }
      }
    }
    arrivals.sort(function (a, b) {
      if (a.time < b.time){return -1;}
      if (a.time > b.time){return +1;}
      return 0;
    });
  }

  function toTime(epochTimeUTC) {
    var time = new Date(0);
    const milliseconds = Math.floor((epochTimeUTC - Math.floor(epochTimeUTC))*1000.);
    time.setUTCSeconds(Math.floor(Number(epochTimeUTC))); 
    time.setMilliseconds(milliseconds);

    var hours = `${time.getHours()}`;
    if (time.getHours() < 10) {
      hours = '0' + hours;
    }
    var minutes = `${time.getMinutes()}`;
    if (time.getMinutes() < 10) {
      minutes = '0' + minutes;
    }
    var seconds = `${time.getSeconds()}`;
    if (time.getSeconds() < 10) {
      seconds = '0' + seconds;
    }
    
    return `${hours}:${minutes}:${seconds}.${time.getMilliseconds()}`;
  }

  function toKey(item) {
    return `${item.network}.${item.station}.${item.channel1}.${item.locationCode}.${item.phase}`;
  }

  function toName(item) {
    if (item.phase == 'S') {
      if (Object.hasOwn(item, 'channel2')) {
        return `${item.network}.${item.station}.${item.channel2}.${item.locationCode}`;
      }
    }
    return `${item.network}.${item.station}.${item.channel1}.${item.locationCode}`;
    
  }

  return (
    <React.Fragment>
      { /* <EventStationMap event={event} stations={stationsToPlot} height={mapHeight} width={mapWidth} /> */ }
      <HStack w='100%' h={height}>
        <Box w='40%' h='98%' bg='white'>
          <Table.ScrollArea borderWidth="1px" maxW="xl" height='100%'>
            <Table.Root size="sm" variant="outline" striped>
              <Table.Header>
                <Table.Row>
                  <Table.ColumnHeader>Name</Table.ColumnHeader>
                  <Table.ColumnHeader>Phase</Table.ColumnHeader>
                  <Table.ColumnHeader>Residual</Table.ColumnHeader>
                  <Table.ColumnHeader>Time</Table.ColumnHeader>
                </Table.Row>
              </Table.Header>
              <Table.Body>
                {
                 arrivals.map( (item) => (
                   <Table.Row key={toKey(item)}>
                     <Table.Cell>{toName(item)}</Table.Cell>
                     <Table.Cell>{item.phase}</Table.Cell>
                     <Table.Cell>{item.residual.toFixed(3)}</Table.Cell>
                     <Table.Cell>{strftimeUTC('%H:%M:%S.%fZ', item.time)}</Table.Cell>
                   </Table.Row>
                 ))
               }
              </Table.Body>
            </Table.Root>
          </Table.ScrollArea>
        </Box>
        <Box w='60%' h='98%'>
          <EventStationMap event={event} stations={stationsToPlot} height='100%' width='100%' />
        </Box>
      </HStack>
    </React.Fragment>
  );
}

export default LocationReview;
