<h1 align="center">
  Seizure Detection and Caregiver Alert System Using TinyML
</h1>

<p align="center">
<img src="images/01-trimimg-upscaled_tinyml_seizure_poster (1).png" width="1100">
</p>

A learning-focused Edge AI project demonstrating how machine learning models can be trained with Edge Impulse, deployed on an ESP32-C3, and integrated into an IoT alerting pipeline.

## Problem Identification

Through a Human-Centered Design approach, I explored the challenges faced by people living with epilepsy. Seizures can occur unexpectedly, increasing the risk of injury, especially when individuals are alone or asleep. Many patients and caregivers lack access to affordable, continuous monitoring solutions that can provide immediate assistance during seizure events.

To better understand the problem, I considered the following questions:

- How might we detect seizures using wearable technology?
- How might we notify caregivers quickly without constant supervision?

After reviewing existing monitoring solutions, I found that many are expensive, require subscriptions, or are impractical for everyday use. This led to the development of a wearable seizure detection and alert system that combines motion and physiological sensing with machine learning.

<p align="center">
<img src="images/Screenshot 2026-06-03 222546.png" width="600">
</p>

### Project Scope

This project focuses specifically on **tonic-clonic seizures**. Seizure symptoms and patterns can vary significantly between individuals and across different seizure types. Due to these variations and the complexity of epilepsy, accurately detecting all seizure types would require larger datasets, personalized models, and additional clinical validation.Tonic-clonic seizures were selected because they typically exhibit distinct movement patterns that can be detected using wearable sensors such as accelerometers and gyroscopes.

### Project Objective

To develop an affordable, wearable, and real-time monitoring system capable of detecting **tonic-clonic seizure events** and automatically notifying caregivers through MQTT, Node-RED, and Telegram alerts, helping improve patient safety and response time.

<p align="center">
<img src="images/Screenshot 2026-06-03 223746.png" width="700">
</p>

## Solution
 
We built a lightweight, wearable IoT device that detects seizures in real time using Edge AI — no cloud dependency, no delay.
The device continuously reads motion data from an MPU6050 sensor (accelerometer + gyroscope) amd heart rate sensor MAX30102 worn on the wrist or arm. A trained neural network runs entirely on the ESP32-C3 microcontroller, classifying every 2-second window of motion data as a seizure or normal activity. If the confidence score crosses **0.85**, the device immediately publishes an alert over MQTT, which Node-RED picks up and forwards as a **Telegram message to the caregiver**.
The result is an end-to-end pipeline — from body motion to caregiver notification — that works in seconds, runs offline for AI inference, and costs under $10 in hardware.

![Seizure Detection Flow](images/trimimg-upscaled_seizure_detection_flow_v2.png)

## Hardware
<div align="center">
  
| Component | Role | Key Specs |
|-----------|------|-----------|
| **ESP32-C3** | Main microcontroller | 32-bit RISC-V, WiFi, runs Edge Impulse model |
| **MPU6050** | Motion sensing | 3-axis accelerometer + 3-axis gyroscope, I2C, 100 Hz |
| **MAX30102** | Vital signs sensing | PPG-based SpO₂ + heart rate monitor, I2C |
| **LiPo Battery** | Power supply | 3.7V, wearable-friendly, rechargeable |
  
</div>

## Dataset Source

The machine learning model used in this project was trained using seizure-related physiological data obtained from Seer Medical's research platform.

Seer Medical is a healthcare technology company specializing in AI-assisted epilepsy monitoring, seizure detection, and long-term neurological data analysis. The dataset provided a valuable reference for understanding seizure-related patterns in wearable sensor data and supporting the development of the Edge Impulse model used in this project.

### Data Provider

- Seer Medical: : https://app.seermedical.com/au

### Usage in This Project

The dataset was utilized during the data preparation and model development phases to:

- Analyze seizure-related motion patterns
- Study physiological signal variations
- Train and evaluate the Edge Impulse classification model
- Improve seizure detection performance using wearable sensor data

> **Note:** This project is an academic and research-oriented prototype intended for educational purposes. It is not a certified medical device and should not be used for clinical diagnosis or treatment decisions.

## Edge Impulse Model

This project uses an Edge Impulse machine learning model deployed directly on the ESP32-C3 for real-time seizure detection. By combining motion and vital-sign data, the system can identify seizure-like events and trigger immediate alerts.

### Data Collection & Training

Data was collected from the following sensors:

- **MPU6050** — 3-axis Accelerometer and 3-axis Gyroscope
- **MAX30102** — Heart Rate and SpO₂ Sensor

The collected samples were uploaded to Edge Impulse, labeled, and used to train a neural network capable of distinguishing between normal activity and seizure activity.

### Edge Impulse Machine Learning Pipeline

The figure illustrates the machine learning workflow used in Edge Impulse. Raw sensor data is collected and cleaned before undergoing feature extraction and engineering. A machine learning model is then trained, tested, and iteratively refined to improve performance. Once validated, the model is deployed to the ESP32-C3 for real-time inference, enabling on-device seizure detection without relying on cloud processing.

<p align="center">
<img src="https://mintcdn.com/edgeimpulse/LSbqkaU8tx8Cie9-/.assets/images/edge-ai-lifecycle-ml-pipeline.png?w=1100&fit=max&auto=format&n=LSbqkaU8tx8Cie9-&q=85&s=c34a9eca098ff106b30025c1ca3138a7" width="600">
</p>

## Model Training

<p align="center">
  <img src="images/Screenshot 2026-06-03 224218.png" width="600">
</p>

<p align="center">
  Edge Impulse dashboard showing performance matric.
</p>

### Deployment

After training, the model was exported as an Arduino library and integrated into the ESP32-C3 firmware.

**Benefits of on-device inference:**

- Real-time seizure detection
- Low latency response
- Offline operation
- Reduced cloud dependency
- Lower power consumption

### Inference Pipeline

```text
MPU6050 + MAX30102
         │
         ▼
 Data Collection
         │
         ▼
 Feature Extraction
         │
         ▼
 Edge Impulse Model
         │
         ▼
 Seizure / Normal
         │
         ▼
 MQTT Alert
         │
         ▼
 Node-RED
         │
         ▼
 Telegram Notification
```

### Example Inference Output

```text
Prediction Scores

Seizure : 0.94
Normal  : 0.06

Result: Seizure Detected
```

The prediction is generated locally on the ESP32-C3. When the seizure confidence exceeds the defined threshold, an MQTT alert is published and automatically forwarded to Telegram through Node-RED.

## MQTT & Node-RED Setup

Follow these steps to receive seizure alerts from the ESP32-C3 and forward them to Telegram using Node-RED.

### 1. Install MQTT Broker (Mosquitto)

```bash
sudo apt update
sudo apt install mosquitto mosquitto-clients -y
sudo systemctl enable mosquitto
sudo systemctl start mosquitto
```

Verify that Mosquitto is running:

```bash
sudo systemctl status mosquitto
```

---

### 2. Install Node-RED

```bash
sudo npm install -g --unsafe-perm node-red
```

Start Node-RED:

```bash
node-red
```

Open the editor:

```text
http://localhost:1880
```

---

### 3. Install Telegram Nodes

In Node-RED:

**Menu → Manage Palette → Install**

Search and install:

```text
node-red-contrib-telegrambot
```

---

### 4. Create a Telegram Bot

1. Open Telegram.
2. Search for **@BotFather**.
3. Send:

```text
/newbot
```

4. Follow the instructions and save the Bot Token.

---

### 5. Build the Node-RED Flow

Add the following nodes:

```text
MQTT In → JSON → Switch → Function → Telegram Sender
```

#### MQTT In

```text
Server : localhost
Port   : 1883
Topic  : seizure/alert
```

#### Switch

```text
Property : msg.payload.seizure
Rule     : is true
```

#### Function

```javascript
msg.payload = {
    chatId: "YOUR_CHAT_ID",
    type: "message",
    content: "🚨 Seizure Detected! Immediate attention required."
};

return msg;
```

Replace `YOUR_CHAT_ID` with your Telegram chat ID.

---

### 6. Deploy

Click **Deploy** in the top-right corner of Node-RED.

---

### 7. Test MQTT Communication

Open a terminal and subscribe to the topic:

```bash
mosquitto_sub -t seizure/alert
```

Publish a test alert from another terminal:

```bash
mosquitto_pub -t seizure/alert -m '{"seizure":true}'
```

If everything is configured correctly:

- MQTT receives the message
- Node-RED processes the alert
- Telegram sends a notification

Your system is now ready to receive seizure alerts from the ESP32-C3.
 
Open Node-RED at `http://localhost:1880`.
 
When the Edge Impulse model detects a seizure event, the ESP32-C3 publishes an MQTT alert. Node-RED receives the message and instantly sends a Telegram notification to the configured caregiver or emergency contact.

## Expected Outcome

The proposed system aims to provide:

- Early detection of tonic-clonic seizure events
- Real-time caregiver notifications
- Low-cost wearable monitoring
- Continuous operation using embedded AI

By combining motion and physiological sensors with machine learning, the system demonstrates the potential of wearable technology for epilepsy monitoring.

## Learning Objectives

This project was developed as a learning exercise to explore the intersection of Embedded AI, IoT, and real-time communication systems.

Through this project, I gained hands-on experience with:

- Collecting and processing sensor data from wearable devices
- Building and deploying machine learning models using Edge Impulse
- Running AI inference on an ESP32-C3 edge device
- Using MQTT for lightweight IoT communication
- Creating automation workflows with Node-RED
- Integrating Telegram for real-time notifications
- Designing end-to-end IoT data pipelines

The primary goal was not to develop a clinical-grade medical device, but to understand how edge machine learning, sensor data, and IoT communication can work together to create intelligent monitoring systems.

## Limitations

- The project focuses only on tonic-clonic seizures.
- Seizure patterns vary significantly between individuals.
- The model requires larger and more diverse datasets for improved generalization.
- This prototype has not undergone clinical validation.
- The system is intended for research and educational purposes only.