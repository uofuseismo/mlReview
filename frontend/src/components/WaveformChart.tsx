import React, { Component } from 'react';
import { render } from 'react-dom';
import HighchartsReact from 'highcharts-react-official';
import Highcharts from 'highcharts';
import HighchartsBoost from 'highcharts/modules/boost';
import { Center } from '@chakra-ui/react';
HighchartsBoost(Highcharts);

import { getDistance } from 'geolib';

const lineColor = 'black';
const lineWidth = 0.8;

const stateOptions: Highcharts.Options = {
  accessibility: {
    enabled: false,
  },
  chart: {
    zooming: {
      type: 'x',
    },
    panning: true,
    panKey: 'shift',
    //marginLeft: 50,
    //marginTop: 0,
  },
  boost: {
    useGPUTranslations: false,
    seriesThreshold: 10,
  },
  title: null,
  subtitle: null,
  credits: {
    enabled: false,
    href: undefined,
  },
  legend: {
    enabled: false,
  },
  tooltip: {
    enabled: true,
    //valueDecimals: 2,
    formatter: function() {
      //const time = new Date(this.x).toISOString();
      //return `${this.series.name}<br>${time}`;
      return `${this.series.name}`;
    },
    //xDateFormat: '%H:%M:%S.%L',
  },
  xAxis: {
    gridLineWidth: 1,
    minorGridLineWidth: 0.5,
    type: 'datetime',
    title: {
      text: undefined,
    },
    labels: {
      enabled: true,
    },
    //ordinal: false,
  },
  yAxis: {
    visible: false,
    min:-1,
    max: 1,
  }, 
  plotLines: [],
  series: [],
  plotOptions: {
    marker: {
      enabled: false,
    },
    series: {
      enableMouseTracking: true,
    },
//    series: {
//      point: {
//        events: null
//      }
//    }
  },
//  responsive: {
//    rules: [{
//      condition: {
//        minWidth: 200,
//        minHeight: 50,
//      },
//      chartOptions: {
//        legend: {
//          enabled: false,
//        },
//      },
//    }],
//  },
};

//function getData(n) {
//    const arr : Array<Number> = [];
//    let i,
//        x,
//        a,
//        b,
//        c,
//        spike;
//    for (
//        i = 0, x = Date.UTC(new Date().getUTCFullYear(), 0, 1) - n * 36e5;
//        i < n;
//        i = i + 1, x = x + 36e5
//    ) {
//        if (i % 100 === 0) {
//            a = 2 * Math.random();
//        }
//        if (i % 1000 === 0) {
//            b = 2 * Math.random();
//        }
//        if (i % 10000 === 0) {
//            c = 2 * Math.random();
//        }
//        if (i % 50000 === 0) {
//            spike = 10;
//        } else {
//            spike = 0;
//        }
//        arr.push([
//            x,
//            2 * Math.sin(i / 100) + a + b + c + spike + Math.random()
//        ]);
//    }
//    return arr;
//}

function segmentToWaveform( segment : object ) {
  var timeSeries : Array<number> = [];
  const floorStartTimeMilliSeconds : number = segment.startTime*1000;
  const samplingPeriodMilliSeconds : number = 1000.0/segment.samplingRate;
  const nSamples = segment.data.length;
  for (var i : number = 0; i < nSamples; i++) {
    const offset : number = i*samplingPeriodMilliSeconds;
    //const time = new Date(floorStartTimeMilliSeconds + offset);
    //timeSeries.push([time, segment.data[i]]); 
    timeSeries.push([floorStartTimeMilliSeconds + offset, segment.data[i]]);
  }
  return timeSeries;
} 

function segmentsToWaveform( segments : Array<object> ) {
  const nSegments = segments.length;
  const mergedSegments : Array<object> = []; 
  var allTimeSeries = [];
  for (var i = 0; i < nSegments; i++) {
    const timeSeries = segmentToWaveform(segments[i]);
    if (timeSeries.length > 0) {
      allTimeSeries = allTimeSeries.concat(timeSeries);
    }
  };
  // Rescale
  const yMin = Math.min(...allTimeSeries.map(item => item[1]));
  const yMax = Math.max(...allTimeSeries.map(item => item[1])); //reduce( (a, b) => Math.max(a[1], b[1]), -Infinity);
  if (Math.abs(yMax - yMin) > 1.e-4) {
    const rescale = 1./(yMax - yMin);
    allTimeSeries = allTimeSeries.map( item => [item[0], 2*(item[1] - yMin)*rescale - 1] );
  }
  console.debug(`Rescaling values are: [${yMin}, ${yMax}]`);
  return allTimeSeries;
}

function computeDistance(network : string,
                         station : string,
                         eventLatitude : number,
                         eventLongitude : number,
                         stationData : []) {
  var distance = undefined;
  const sourcePoint = {latitude : eventLatitude, longitude : eventLongitude};
  for (var i = 0; i < stationData.length; i++) {
    if (network === stationData[i].network &&
        station === stationData[i].name) {
      const destinationPoint
        = { latitude: stationData[i].latitude,
            longitude: stationData[i].longitude };
      distance = getDistance( sourcePoint, destinationPoint );
      break;
    }
  }
  if (distance === undefined) {
    console.warn(`Could not find distance for ${network}.${station}`);
  }
  return distance;
}

function computePlotOrder(waveformData, stationData, eventData) {
 const eventLatitude = eventData.origin.latitude;
 const eventLongitude = eventData.origin.longitude;
 for (var iWaveform = 0; iWaveform < waveformData.length; iWaveform++) {
   const distance = computeDistance(waveformData[iWaveform].network,
                                    waveformData[iWaveform].station,
                                    eventLatitude, eventLongitude,
                                    stationData);
   waveformData[iWaveform]['distance'] = Math.floor(distance);
 }
}

function WaveformChart( {waveformData, stationData, eventData} ) {
  //const chartComponentReference = React.useRef<Highcharts.React.RefObject> (null);
  //ref={chartComponentReference}
  computePlotOrder(waveformData, stationData, eventData);
  waveformData.sort(function (a, b) {
    if (a.distance < b.distance){return +1;}
    if (a.distance > b.distance){return -1;}
    if (a.name < b.name){return -1;}
    if (a.name > b.name){return +1;}
    return 0;
  }); 

  const arrivals = eventData.origin.arrivals;
  const nArrivals = arrivals.length;
  const nWaveforms = waveformData.length;
  const nPixels = 100*Math.max(1, (nWaveforms - 1));
  const plotHeight = `${nPixels}px`;
  console.debug(`Will plot ${nWaveforms} waveforms`);
  var [hoverData, setHoverData] = React.useState(null);
  var [chartOptions, setChartOptions]
    = React.useState<Highcharts.Options> (stateOptions); 

  //chartOptions.subtitle.text = waveformName;
  //console.log(arrivals);
  chartOptions.chart.height = nPixels;
  chartOptions.yAxis.max = Math.max(1, 2*(nWaveforms - 1) + 1);
  chartOptions.series = [];
  for (var iWaveform = nWaveforms - 1; iWaveform >= 0; iWaveform--)
  {
    const network = waveformData[iWaveform].network;
    const station = waveformData[iWaveform].station;
    const channel = waveformData[iWaveform].channel;
    const locationCode
        = waveformData[iWaveform].locationCode ?
          '.' + waveformData[iWaveform].locationCode : '';
    const waveformName = waveformData[iWaveform].name;
    //console.log(waveformData[iWaveform].distance, waveformName);
    const waveform = segmentsToWaveform(waveformData[iWaveform].segments);
    const shiftedWaveform = waveform.map( item => [item[0], item[1] + 2*iWaveform] );
//console.log(shiftedWaveform[0]);
//console.log(shiftedWaveform[shiftedWaveform.length - 1]);
//console.log(waveform);
    const startTime = waveformData[iWaveform].startTime;
    const endTime = waveformData[iWaveform].endTime;
    for (var iArrival = 0; iArrival < nArrivals; ++iArrival) {
      if (arrivals[iArrival].network === network &&
          arrivals[iArrival].station === station && 
          arrivals[iArrival].time >= startTime &&
          arrivals[iArrival].time <= endTime) {
        var pickLineColor = 'blue';
        var dashStyle = 'ShortDash';
        if (arrivals[iArrival].phase === 'P') {
          pickLineColor = 'red';
          if (channel[2] === 'Z') {
            dashStyle = 'Solid';
          }
          else {
            continue;
          }
        }
        else {
          if (channel[2] != 'Z') {
            dashStyle = 'Solid';
          }
          else {
            continue;
          }
        }
         
        const pickTime = arrivals[iArrival].time*1000;
        chartOptions.series.push(
          {
            name: `${waveformName} ${arrivals[iArrival].phase}`,
            type: 'line',
            linewidth: 0.5,
            dashstyle: dashStyle,
            color: pickLineColor,
            data: [ [pickTime, 2*iWaveform - 0.9], [pickTime, 2*iWaveform + 0.9] ],
          }
        );
      }
    }

    chartOptions.series.push(
      {
        name: `${waveformName}`,
        description: `Waveform for ${waveformName}`,
        type: 'line',
        boostThreshold: 1000,
        linewidth: lineWidth,
        color: lineColor,
        data: shiftedWaveform,
      },
    );
  }
//  chartOptions.series[0].data = waveform; //getData(20000);

//  chartOptions.plotOptions.series.point.events = {
//    mouseOver(e) {
//      //console.log(e.target.x, e.target.y);
//      setHoverData(e);
//    }
//  };

//console.log(waveform);
//  const updateWaveform = () => {
//    setChartOptions( {
//      series: [
//        { data: waveform },
//      ]
//    });
//  }

  return (
    <React.Fragment>
      <Center w='100%' h='100%' overflow='auto'>
        <HighchartsReact
          containerProps={{ style: { width: "98%", height: plotHeight } }}
          highcharts={Highcharts}
          options={chartOptions}
        />
      </Center>
    </React.Fragment>
  )   
}

export default WaveformChart;
