export enum ReviewStatus {
  Automatic = 'Automatic',
  Human = 'Human',
  Unknown = 'Unknown', 
};

export interface Pick {
  phase : string,
  time : number,
  reviewStatus : ReviewStatus,
};

export interface Segment {
  data : Array<number>,
  startTime : number,
  samplingRate : number
};

export interface Waveform {
  segments : Array<Segment>,
  name : string,
  network : string,
  station : string,
  channel : string,
  locationCode : string,
  startTime : number,
  endTime : number
};

function unpackSegment<Segment> ( json : object ) {
  var result : Segment = {};

  //var startTime = new Date(0);
  //const startTimeSeconds = Number(json.startTimeMuS)*1.e-6;
  //startTime.setUTCSeconds(Math.round(startTimeSeconds*1000));

  if (json.data != null) {
    if (json.data.length > 0) {
      result.startTime = json.startTimeMuS*0.000001;
      result.samplingRate = json.samplingRateHZ;
      result.endTime = result.startTime + (json.data.length - 1)/result.samplingRate;
      result.data = json.data;
    }
  }

  return result;
}

function unpackWaveform<Waveform> ( json : object ) {

  var result : Waveform = {};
  result.network = json.network;
  result.station = json.station;
  result.channel = json.channel;
  if (Object.hasOwn(json, 'locationCode')) {
    result.locationCode = json.locationCode;
    result.name = result.network + '.'
                + result.station + '.'
                + result.channel + '.'
                + result.locationCode;
  }
  else {
    result.locationCode = null;
    result.name = result.network + '.'
                + result.station + '.'
                + result.channel + '.'
                + '--';
  }
  var startTime = undefined; 
  var endTime = undefined;
  if (Object.hasOwn(json, 'segments'))
  {
    var segments : Array<Segment> = [];
    for (var i = 0; i < json.segments.length; i++) {
      const segment = unpackSegment(json.segments[i]); 
      if (Object.hasOwn(segment, 'data')) {
        if (startTime === undefined) {
          startTime = segment.startTime;
        }
        if (endTime === undefined) {
          endTime = segment.endTime;
        }
        startTime = Math.min(startTime, segment.startTime); 
        endTime = Math.max(endTime, segment.endTime);
        segments.push(segment);
      }
    }
    result.segments = segments;
  }
  result.startTime = startTime;
  result.endTime = endTime;
  return result;
}

function unpackWaveforms( json : object ) : Array<Waveform> {
  var waveforms = [];
  for (var i = 0; i < json.data.length; i++) {
    const waveform = unpackWaveform(json.data[i]);
    if (Object.hasOwn(waveform, 'segments')) {
      waveforms.push(waveform);
    }
  }
  return waveforms;
}

export { unpackWaveform, unpackWaveforms };
