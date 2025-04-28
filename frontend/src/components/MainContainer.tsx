import React from 'react';
import Header from '/src/components/Header';
import ReviewPage from '/src/components/ReviewPage';

import { Tabs } from "@chakra-ui/react"
import { Box, Button, Center, HStack, VStack, Flex } from '@chakra-ui/react';

import { IconType } from 'react-icons';
import { BiTransferAlt } from 'react-icons/bi';
import { WiEarthquake } from "react-icons/wi";
import { IconButton, Image } from '@chakra-ui/react';
import { Avatar } from '/src/components/ui/avatar';
import { UserCredentials } from '/src/utilities/userCredentials';

interface LinkItemProperties {
  name : string,
  icon: IconType,
};

const LinkItems: Array<LinkItemPropertiesj> = [
  { name: 'Events', icon: WiEarthquake},
  { name: 'Data', icon: BiTransferAlt},
];

function SidebarContent( {width, height, onHandleButtonSelection} ) {
  return (
    <React.Fragment>
      <Box
       as="nav"
       pos="fixed"
       top="0"
       left="0"
       zIndex="sticky"
       h={height}
       pb="10"
       overflowX="hidden"
       overflowY="auto"
       bg="gray.800"
       borderColor="black"
       borderRightWidth="1px" 
       width={width}
      >
        <Flex h="30" direction="column" alignItems="center" gap="1">
          <Image
           src="/src/assets/uussWiggle.png"
           alt="Univeristy of Utah Logo"
           border="1px solid black"
           h="100%" w="98%"
           rounded="sm"
           bg="gray.800"
          />
          <Button
           borderRadius={10}
           variant="solid"
           backgroundColor="black"
           color="white"
           width="full"
           _hover={{borderColor: "red.700"}}
           onClick={() => {
                           console.debug(`Switching to events view...`);
                           onHandleButtonSelection('events');
                          }}
          >   
            Events <WiEarthquake />
          </Button>
          <Button borderRadius={10}
                  variant="solid"
                  backgroundColor="black"
                  color="white"
                  width="full"
                  outline='gray'
                  _hover={{borderColor: "red.700"}}
           onClick={() => {
                           console.debug(`Switching to data view...`);
                           onHandleButtonSelection('data');
                          }}
          >
            Data <BiTransferAlt />
          </Button>
        </Flex>
      </Box>
    </React.Fragment>
  );
}

function MainContainer( { userCredentials } : { UserCredentials } ) {

  var [mainComponentName, setMainComponentName] = React.useState('default');

  const MainComponent = () => {
    if (mainComponentName === 'events') {
      console.debug('Main component is events review page');
      return (
        <React.Fragment> 
          <ReviewPage userCredentials={userCredentials} />
        </React.Fragment>
      );
    }
    else if (mainComponentName === 'data') {
      console.debug('Main component is data viewer');
      return (
        <React.Fragment>
        </React.Fragment>
      );
    }
    else if (mainComponentName === 'default') {
      console.debug('Main component is default');
      return (
        <React.Fragment>
        </React.Fragment>
      );
    }
    else {
      console.warn(`Unhandled component ${mainComponentName}`);
      return (
        <React.Fragment>
        </React.Fragment>
      );
    }
  }

  const handleSelectMainComponent = (componentName) => {
    if (componentName != mainComponentName) {
      if (componentName === 'events' ||
          componentName === 'data' ||
          componentName === 'default') {
        setMainComponentName(componentName)
      }
      else {
        console.warn(`Unhandled component name ${mainComonentName}`);
      }
    }
    else {
      console.debug(`${componentName} is currently selected`);
    }
  }

  console.debug('Rendering main container...');
  return (
    <React.Fragment>
      <Flex width="100%" height="100%">
        <SidebarContent
         width="10%"
         height="full"
         onHandleButtonSelection={handleSelectMainComponent}
        />
        <Flex height="100%" width="90%" gap="5" direction="column">
          <Flex
           as="header"
           bg="gray.900"
           align="right"
           pos="fixed"
           top="0"
           left="10%"
           width="100%"
           height="8%"
           minHeight="10vh"
           maxHeight="40vh"
          >
            <Button variant="solid" backgroundColor="black">
              <Avatar variant="outline" color="white" name="Ben Baker"/>
            </Button>
          </Flex>
          <Box as="main" pos="fixed" top="8%" left="10%" width="90%" height="full" bg="gray.100" border='red'>
            <MainComponent />
          </Box>
        </Flex>
      </Flex>
    </React.Fragment>
  );
};

export default MainContainer;
