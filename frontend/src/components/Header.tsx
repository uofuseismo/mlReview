import React from 'react';
import { Center, Flex, Heading, HStack } from '@chakra-ui/react';
import { IoMdSettings } from "react-icons/io";
import { Button } from '/src/components/ui/button';
import { Tooltip } from '/src/components/ui/tooltip';
import EventsDrawer from '/src/components/EventsDrawer';

function Header( {eventIdentifier, eventInformation, onSetSelectedEvent} ) {
  console.debug(`Rendering header ${eventIdentifier}...`);
 const open = true;
 
  return (
    <React.Fragment>
      <Flex color='white' w='100%' h='50px'>
        <Center w='12%' bg='black' color='white'>
          <Heading fontSize='22px' color='white' align='left'>{eventIdentifier}</Heading>
        </Center>
        <Center w='58%' bg='black'/> 
        <Center w='30%' bg='black' color='white'>
          <HStack>
            <EventsDrawer events={eventInformation} onSetSelectedEvent={onSetSelectedEvent} drawerOpen={open} />
            <Tooltip showArrow content="User settings">
              <Button colorPalette="gray" variant="outline">
                <IoMdSettings /> Profile
              </Button>
            </Tooltip>
          </HStack>
        </Center>
      </Flex>
    </React.Fragment>
  );
}

export default Header;
