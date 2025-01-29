import React, { useState, useEffect } from 'react';
import { StyleSheet, View, TouchableOpacity, Alert, Platform } from 'react-native';
import * as Location from 'expo-location';
import * as SMS from 'expo-sms';
import { StatusBar } from 'expo-status-bar';
import { LinearGradient } from 'expo-linear-gradient';
import { Text } from 'react-native';
import { MaterialCommunityIcons } from '@expo/vector-icons';
import * as FileSystem from 'expo-file-system';

export default function App() {
    const [location, setLocation] = useState(null);
    const [loading, setLoading] = useState(false);
    const PHONE_NUMBER = "+237658866639"; // Numéro du microcontrôleur

    useEffect(() => {
        requestLocationPermission();
    }, []);

    const requestLocationPermission = async () => {
        try {
            const { status } = await Location.requestForegroundPermissionsAsync();
            if (status !== 'granted') {
                Alert.alert('Permission refusée', 'La permission de localisation est nécessaire.');
            }
        } catch (error) {
            console.error('Erreur de permission:', error);
        }
    };

    const tryNativeSMS = async (message: string) => {
        if (Platform.OS !== 'android') return false;

        try {
            // Tentative d'utiliser l'API native Android via expo-modules-core
            const SendSMSModule = require('expo-modules-core').NativeModulesProxy.ExpoSMS;
            if (SendSMSModule && SendSMSModule.sendDirectSMS) {
                await SendSMSModule.sendDirectSMS(PHONE_NUMBER, message);
                return true;
            }
        } catch (error) {
            console.log('Native SMS failed:', error);
            return false;
        }
        return false;
    };

    const fallbackToExpoSMS = async (message: string) => {
        const isAvailable = await SMS.isAvailableAsync();
        if (isAvailable) {
            try {
                const { result } = await SMS.sendSMSAsync([PHONE_NUMBER], message);
                return result === 'sent';
            } catch (error) {
                console.log('Expo SMS failed:', error);
                return false;
            }
        }
        return false;
    };

    const getLocationAndSendSMS = async () => {
        setLoading(true);
        try {
            // 1. Récupérer la position
            const location = await Location.getCurrentPositionAsync({
                accuracy: Location.Accuracy.High
            });
            setLocation(location);

            // 2. Préparer le message
            const message = `predict ${location.coords.latitude} ${location.coords.longitude}`;

            // 3. Tentative d'envoi direct (Android uniquement)
            let sent = false;
            if (Platform.OS === 'android') {
                sent = await tryNativeSMS(message);
            }

            // 4. Si l'envoi direct échoue, utiliser expo-sms
            if (!sent) {
                sent = await fallbackToExpoSMS(message);
            }

            if (sent) {
                Alert.alert('Succès', 'Position envoyée avec succès !');
            } else {
                Alert.alert('Note', "L'application de messagerie va s'ouvrir pour envoyer les coordonnées.");
            }
        } catch (error) {
            Alert.alert('Erreur', 'Impossible de récupérer votre position ou d\'envoyer le SMS');
            console.error(error);
        } finally {
            setLoading(false);
        }
    };

    return (
        <View style={styles.container}>
            <StatusBar style="light" />
            <LinearGradient
                colors={['#4c669f', '#3b5998', '#192f6a']}
                style={styles.background}
            >
                <View style={styles.content}>
                    <Text style={styles.title}>NPK Soil Analyzer</Text>
                    <TouchableOpacity
                        style={[styles.button, loading && styles.buttonDisabled]}
                        onPress={getLocationAndSendSMS}
                        disabled={loading}>
                        <MaterialCommunityIcons
                            name={loading ? "timer-sand" : "leaf"}
                            size={32}
                            color="#FFF"
                        />
                        <Text style={styles.buttonText}>
                            {loading ? 'Envoi en cours...' : 'Analyser le sol'}
                        </Text>
                    </TouchableOpacity>
                    {location && (
                        <View style={styles.coordsContainer}>
                            <Text style={styles.coords}>
                                Position actuelle :{'\n'}
                                Latitude: {location.coords.latitude.toFixed(6)}{'\n'}
                                Longitude: {location.coords.longitude.toFixed(6)}
                            </Text>
                            {location.coords.accuracy && (
                                <Text style={styles.accuracy}>
                                    Précision: ±{Math.round(location.coords.accuracy)}m
                                </Text>
                            )}
                        </View>
                    )}
                </View>
            </LinearGradient>
        </View>
    );
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
    },
    background: {
        flex: 1,
    },
    content: {
        flex: 1,
        justifyContent: 'center',
        alignItems: 'center',
        padding: 20,
    },
    title: {
        fontSize: 28,
        fontWeight: 'bold',
        color: '#FFF',
        marginBottom: 40,
        textAlign: 'center',
        textShadowColor: 'rgba(0, 0, 0, 0.3)',
        textShadowOffset: { width: 0, height: 2 },
        textShadowRadius: 4,
    },
    button: {
        backgroundColor: 'rgba(255, 255, 255, 0.2)',
        paddingVertical: 15,
        paddingHorizontal: 30,
        borderRadius: 25,
        flexDirection: 'row',
        alignItems: 'center',
        justifyContent: 'center',
        gap: 10,
        elevation: 3,
        shadowColor: '#000',
        shadowOffset: { width: 0, height: 2 },
        shadowOpacity: 0.25,
        shadowRadius: 3.84,
    },
    buttonDisabled: {
        opacity: 0.7,
    },
    buttonText: {
        color: '#FFF',
        fontSize: 18,
        fontWeight: '600',
    },
    coordsContainer: {
        backgroundColor: 'rgba(255, 255, 255, 0.1)',
        padding: 15,
        borderRadius: 15,
        marginTop: 20,
    },
    coords: {
        color: '#FFF',
        textAlign: 'center',
        opacity: 0.9,
        fontSize: 16,
    },
    accuracy: {
        color: '#FFF',
        textAlign: 'center',
        opacity: 0.7,
        marginTop: 5,
        fontSize: 14,
    },
});