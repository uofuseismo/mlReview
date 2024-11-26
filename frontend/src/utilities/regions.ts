import { isPointInPolygon } from 'geolib';

function getUtahRegion() : Array<Array<number>> {
  const region: Array<Array<number>> = [ 
    [36.75, -114.25],
    [36.75, -108.75],
    [42.50, -108.75],
    [42.50, -114.25],
    [36.75, -114.25]
  ];
  return region;
};

function getYellowstoneRegion() : Array<Array<number>> {
  const region: Array<Array<number>> = [
    [44.00,  -111.333],
    [44.00,  -109.750],
    [45.167, -109.750],
    [45.167, -111.333],
    [44.00,  -111.333]
  ];
  return region;
}

function getReportingRegion() : Array<Array<number>> {
  const region: Array<Array<number>> = [
    [35, -115.25],
    [35, -107.75],
    [46, -107.75],
    [46, -115.25],
    [35, -115.25]
  ];
  return region;
};

function getFORGERegion() : Array<Array<number>> {
  const region: Array<Array<number>> = [
    [38.50766251464152, -112.9172670480094],
    [38.50765095807106, -112.8984475721061],
    [38.48464129404871, -112.8986038105227],
    [38.48473681463327, -112.889197233939],
    [38.50007663264921, -112.8891782562262],
    [38.50006934569299, -112.8800311838714],
    [38.51489823672957, -112.8801413665903],
    [38.51492363342575, -112.9169998098031],
    [38.50766251464152, -112.9172670480094]
  ];
  return region;
};

function isInRegion(regionName : string, latitude : number, longitude : number) {
  var region = [];
  const queryPoint = {latitude : latitude, longitude : longitude};
  if (regionName === 'utah') {
    region = getUtahRegion();
  }
  else if (regionName === 'yellowstone') {
    region = getYellowstoneRegion();
  } 
  else if (regionName === 'reporting') {
    region = getReportingRegion();
  }
  else {
    console.error(`Unhandled region ${regionName}`);
    return false;
  }
  const boundary = region.map( (x) => (
    {
      latitude : x[0],
      longitude : x[1]
     }
  ) );
  return isPointInPolygon(queryPoint, boundary);
}

function isInUtahRegion(latitude : number, longitude : number) {
  return isInRegion('utah', latitude, longitude);
}

function isInYellowstoneRegion(latitude : number, longitude : number) {
  return isInRegion('yellowstone', latitude, longitude);
}

function isInReportingRegion(latitude : number, longitude : number) {
  return isInRegion('reporting', latitude, longitude);
}

function isInAuthoritativeRegion(latitude : number, longitude : number) {
  if (isInUtahRegion(latitude, longitude)){return true;}
  if (isInYellowstoneRegion(latitude, longitude)){return true;}
  return false;
}
    
export { getFORGERegion, getReportingRegion, getUtahRegion,  getYellowstoneRegion, isInUtahRegion, isInYellowstoneRegion, isInAuthoritativeRegion };

