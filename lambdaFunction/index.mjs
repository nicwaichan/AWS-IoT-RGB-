import {
    IoTDataPlaneClient,
    PublishCommand,
} from "@aws-sdk/client-iot-data-plane";

const client = new IoTDataPlaneClient({ 
    endpoint: 'https://a11cku7s7fh0nk-ats.iot.us-east-1.amazonaws.com',
});

export const handler = async(event) => {
    let rgb = '';
    
    if (event.queryStringParameters && event.queryStringParameters.rgb) {
        rgb = event.queryStringParameters.rgb;
    }
    
    await client.send(
        new PublishCommand({
            topic: "arduino/incoming",
            // payload: rgb,
            // payload: '"RGB": "ffff"',
            payload: ' {"RGB":'+ '"' + rgb + '"}',
            qos: 1,
        })
    )
    const response = {RGB:rgb};
    
    return response;
};

// https://bu3capc3vk.execute-api.us-east-1.amazonaws.com/ppa?rgb=100,111,222
