import React from 'react';
//import getAsyncEventTableData from '/src/utilities/loadEventTableData';
import { eventToEventInformation, EventType, EventInformation, GeographicType, ReviewStatus } from '/src/utilities/eventTypes';
import Header from '/src/components/Header';
import WaveformChart from '/src/components/WaveformChart';
import LocationReview from '/src/components/LocationReview';
import Summary from '/src/components/Summary';

import getAsyncStationData from '/src/utilities/loadStationData';
import { stationToStationInformation, StationInformation } from '/src/utilities/stationTypes';

import { Tabs } from "@chakra-ui/react"
import { Box, Button, Center, HStack, VStack, Flex } from '@chakra-ui/react';

//import allWaveforms from '/src/data/waveforms.14621.json';
//import testData from '/src/data/yhh.json';
import { unpackWaveforms } from '/src/utilities/waveformTypes';

import fetchAsyncWaveformsFromAPI from '/src/utilities/fetchWaveformData';
import fetchAsyncEventsHashFromAPI from '/src/utilities/fetchEventsHash';
import fetchAsyncEventsFromAPI from '/src/utilities/fetchEvents';

const catalogRefreshRate : number = 60;

//var waveformData = unpackWaveforms(allWaveforms);

function ReviewPage( { userCredentials } : { UserCredentials } ) {

//fetchAsyncWaveformsFromAPI('15351');

  var [eventInformation, setEventInformation] = React.useState<EventInformation[]> ([]);
  var [eventHash, setEventHash] = React.useState<number> (0);
  var [stationInformation, setStationInformation] = React.useState<StationInformation[]> ([]);

  var [currentEvent, setCurrentEvent] = React.useState<EventInformation> (null);
  var [currentEventIdentifier, setCurrentEventIdentifier] = React.useState<string> ('');
  var [currentWaveforms, setCurrentWaveforms] = React.useState<Waveform[]> ([]);

  function getLatestEvents() {
    console.debug('Querying for latest events hash...');
    fetchAsyncEventsHashFromAPI(userCredentials.jsonWebToken
                                //userCredentials.name,
                                //userCredentials.password
                               ).then(hashResult => {
      if (hashResult.statusCode == 200) {
        if (hashResult.data.hash != eventHash) {
          console.debug('New hash detected; getting new events');
          fetchAsyncEventsFromAPI(userCredentials.jsonWebToken//,
                                  //userCredentials.name,
                                  //userCredentials.password
                                 ).then(result => {
            if (result.statusCode == 200) {
              var newEvents : EventInformation[] = []; 
              for (var i = 0; i < result.data.events.length; i++) {
                newEvents.push(eventToEventInformation(result.data.events[i]));
              }
              console.info(`Setting ${newEvents.length} events...`);
              setEventInformation(newEvents);
              setEventHash(result.data.hash);
            }
          })
          .catch(error => {
            console.error(`Failed to get events; failed with ${error}`);
          });
        }
        else {
          console.debug('No catalog update');
        }
      }
      else {
          console.warn(`Error in getting events hash (statusCode=${hashResult.statusCode})`);
      }
    })
    .catch(error => {
      console.error(`Failed to get events hash; failed with ${error}`);
    });
  }

  getLatestEvents();

  React.useEffect( () => {
    const timer = setInterval(getLatestEvents, catalogRefreshRate*1000);
    return () => clearInterval(timer);
  }, []);


  React.useEffect( () => {
    getAsyncStationData().then(result => {
      result = result.payload;
      if (result.statusCode == 200) {
        var newStations : StationInformation[] = [];
        for (var i = 0; i < result.data.length; i++) {
          newStations.push(stationToStationInformation(result.data[i]));
        }
        console.debug(`Will add ${newStations.length} stations`);
        setStationInformation(newStations);
      }
      else {
        console.warn(`Failed to get station data`);
      }
    })
    .catch(error => {
      console.error(error);
    });
  }, []);

  //React.useEffect( () => {
  //  //getAsyncEventTableData().then(result => {
  //  fetchAsyncEventsFromAPI(userCredentials.name,
  //                          userCredentials.password).then(result => {
  //    //result = result.payload;
  //    if (result.statusCode == 200) {
  //      var newEvents : EventInformation[] = [];
  //      for (var i = 0; i < result.data.events.length; i++) {
  //        newEvents.push(eventToEventInformation(result.data.events[i]));
  //      }
  //      console.log(`Setting ${newEvents.length} events...`);
  //      setEventInformation(newEvents);
  //      setEventHash(result.data.hash);
  //    }
  //  });
  //}, []);

  const handleSetSelectedEvent = (eventIdentifier) => {
    console.debug(`Switching to ${eventIdentifier}...`);
    const thisEvent = eventInformation.find( function(event) {
      return event.identifier === eventIdentifier;
    });
    if (thisEvent === undefined) {
      console.warn(`Could not find ${eventIdentifier}`);
    }
    else {
      console.debug(`Found ${eventIdentifier}`);
      if (currentEvent) {
        if (thisEvent.identifier != currentEvent.identifier) {
          setCurrentEvent(thisEvent);
          setCurrentEventIdentifier(eventIdentifier);
          var waveformData = [];
          fetchAsyncWaveformsFromAPI(eventIdentifier,
                                     userCredentials.jsonWebToken).then(result => {
            waveformData = unpackWaveforms(result);
            setCurrentWaveforms(waveformData);
          });
        }
      }
      else {
        setCurrentEvent(thisEvent);
        setCurrentEventIdentifier(eventIdentifier);
        var waveformData = []; 
        fetchAsyncWaveformsFromAPI(eventIdentifier,
                                   userCredentials.jsonWebToken).then(result => {
          waveformData = unpackWaveforms(result);
          setCurrentWaveforms(waveformData);
        });
      }
    }
  };

  console.debug('Rendering review page');
  return (
    <React.Fragment>
      <VStack h="100%" w="100%">
        <Header
         eventIdentifier={currentEventIdentifier}
         eventInformation={eventInformation}
         onSetSelectedEvent={handleSetSelectedEvent}
         height="10%"
        />
        <Center w="100%" h="90%">
          <Tabs.Root
           defaultValue="summary"
           orientation='horizontal'
           lazyMount={true}
           unmountOnExit={true}
           align='left'
           variant='enclosed'
           justify='center'
           w='100%'
           h='100%'
          >
          <Tabs.List h='20px'>
            <Tabs.Trigger value="summary">Summary</Tabs.Trigger>
              <Tabs.Trigger value="waveforms" disabled={currentEvent === null ? true : false}>Waveforms</Tabs.Trigger>
              <Tabs.Trigger value="location"  disabled={currentEvent === null ? true : false}>Location</Tabs.Trigger>
            </Tabs.List>
            <Tabs.Content value="summary">
              <Summary event={currentEvent} />
            </Tabs.Content>
            <Tabs.Content value="waveforms" height="85%">
              <Box w="100%" overflowY="auto" height="100%">
              <WaveformChart waveformData={currentWaveforms} stationData={stationInformation} eventData={currentEvent} height="100%"/>
            </Box>
            </Tabs.Content>
            <Tabs.Content value="location" height="85%">
              <LocationReview event={currentEvent} stations={stationInformation} height="100%"/>
            </Tabs.Content>
          </Tabs.Root>
        </Center>
      </VStack>
    </React.Fragment>
  );
};

export default ReviewPage;
