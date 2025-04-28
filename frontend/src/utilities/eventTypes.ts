
export interface ArrivalInformation {
  network : string,
  station : string,
  channel1 : string,
  channel2?: string,
  channel3?: string, 
  locationCode : string,
  phase : string,
  time : Date,
  residual?: number,
};

export interface OriginInformation {
  identifier?: string,
  time : Date,
  latitude : number,
  longitude : number,
  depth : number,
  arrivals : ArrivalInformation[],
};

export interface EventInformation {
  identifier : string,
  origin : OriginInformation,
  aqmsEventIdentifiers?: string[],
  reviewed : boolean,
};

export function eventToEventInformation<EventInformation>(value : object) {
  if (typeof(value) !== 'object') {
    return null;
  }

  if (!Object.hasOwn(value, 'preferredOrigin')) {
    throw new Error('Event does not have a preferred origin');
  }
  const preferredOrigin = value.preferredOrigin;

  var origin : OriginInformation = {};

  var arrivals : ArrivalInformation = []
  if (Object.hasOwn(preferredOrigin, 'arrivals')) {
    arrivals = preferredOrigin.arrivals.map( function(element) { 
      var arrival : ArrivalInformation = {};
      var arrivalTime = new Date(0);
      const milliseconds = Math.floor((element.time - Math.floor(element.time))*1000.);
      arrivalTime.setUTCSeconds(Math.floor(Number(element.time))); 
      arrivalTime.setMilliseconds(milliseconds);
      arrival.network = String(element.network);
      arrival.station = String(element.station);
      arrival.channel1 = String(element.channel1);
      if (Object.hasOwn(element, 'channel2')) {
        arrival.channel2 = String(element.channel2);
      }
      if (Object.hasOwn(element, 'channel3')) {
        arrival.channel3 = String(element.channel3);
      }
      arrival.phase = String(element.phase);
      if (Object.hasOwn(element, 'locationCode')) {
        arrival.locationCode = String(element.locationCode);
      }
      else {
        arrival.locationCode = '';
      }
      arrival.time = arrivalTime; //Number(element.time); //arrivalTime; 
      if (Object.hasOwn(element, 'residual')) {
        arrival.residual = Number(element.residual);
      }
      return arrival;
    });
  }

  var originTime = new Date(0);
  const milliseconds = Math.floor((preferredOrigin.time - Math.floor(preferredOrigin.time))*1000.);
  originTime.setUTCSeconds(Math.floor(Number(preferredOrigin.time)));
  originTime.setMilliseconds(milliseconds);

  origin.time = originTime;
  origin.latitude = Number(preferredOrigin.latitude);
  origin.longitude = Number(preferredOrigin.longitude);
  origin.depth = Number(preferredOrigin.depth);
  origin.arrivals = arrivals;

  var event : EventInformation = {};

  event.identifier = String(value.eventIdentifier);
  event.origin = origin;
  event.reviewed = Boolean(value.reviewed);

  if (Object.hasOwn(value, 'aqmsEventIdentifiers')) {
    event.aqmsEventIdentifiers = value.aqmsEventIdentifiers.map( (e) =>  String(e) );
  } 

  return event;
};

