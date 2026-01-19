// ############################################################################//
//                                                                             //
//    ... перезагрузить z2m, что бы конвертер применился                       //
//                                                                             //
//#############################################################################//

const {
    battery,
    binary,
    enumLookup,
    numeric,
    temperature,
	soilMoisture,
	illuminance,
	reporting,
	identify,
} = require('zigbee-herdsman-converters/lib/modernExtend');


const definition = {
        zigbeeModel: ["MODKAM_Flower"],
        model: "MODKAM_Flower",
        vendor: "EFEKTA",
        description: "Soil moisture sensor MODKAM_Flower, mod DIYRuZ Flower",
        extend: [
            identify(),
            soilMoisture({
                reporting: false,
                access: "STATE",
            }),
            temperature({
                reporting: false,
                access: "STATE",
            }),
            illuminance({
                reporting: false,
                access: "STATE",
            }),
            numeric({
                name: "reading_interval",
                unit: "min.",
                valueMin: 1,
                valueMax: 360,
                cluster: "genPowerCfg",
                attribute: {ID: 0x0201, type: 0x21},
                description: "Setting the time in minutes, by default 3 minutes",
                access: "STATE_SET",
            }),
            binary({
                name: "smart_sleep",
                valueOn: ["ON", 1],
                valueOff: ["OFF", 0],
                cluster: "genPowerCfg",
                attribute: {ID: 0x0216, type: 0x10},
                description: "Enable Smart Sleep, short wakeup every 7 seconds",
                access: "STATE_SET",
            }),
            enumLookup({
                name: "tx_radio_power",
                lookup: {"-4dbm": -4, "0dbm": 0, "4dbm": 4},
                cluster: "genPowerCfg",
                attribute: {ID: 0x0236, type: 0x28},
                description: "Set TX Radio Power, dbm",
                access: "STATE_SET",
            }),
            numeric({
                name: "lifetime",
                unit: "Hours",
                cluster: "genTime",
                attribute: "standardTime",
                description: "Device lifetime in hours",
                access: "STATE",
            }),
            enumLookup({
                name: "resolution",
                lookup: {"9 bit": 9, "10 bit": 10, "11 bit": 11, "12 bit": 12},
                cluster: "msTemperatureMeasurement",
                attribute: {ID: 0x0520, type: 0x20},
                description: "Set Bit resolution DS18B20",
                access: "STATE_SET",
            }),
            numeric({
                name: "lux_factor",
                valueMin: 1,
                valueMax: 30,
                valueStep: 1,
                cluster: "msIlluminanceMeasurement",
                attribute: {ID: 0x0310, type: 0x29},
                description: "Lux factor",
                access: "STATE_SET",
            }),
            binary({
                name: "temperature_compensation",
                valueOn: ["ON", 1],
                valueOff: ["OFF", 0],
                cluster: "msTemperatureMeasurement",
                attribute: {ID: 0x0504, type: 0x10},
                description: "Temperature compensation",
                access: "STATE_SET",
            }),
			battery({
                percentage: true,
                lowStatus: false,
                voltage: false,
                percentageReporting: false,
            }),
        ],
    };

module.exports = definition;