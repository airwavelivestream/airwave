package com.airwave.cast.discovery

import android.content.Context
import android.net.nsd.NsdManager
import android.net.nsd.NsdServiceInfo
import android.util.Log

class DiscoveryManager(
    context: Context,
    private val onDeviceFound: (name: String, host: String, port: Int) -> Unit
) {
    private val nsdManager = context.getSystemService(Context.NSD_SERVICE) as NsdManager
    private val serviceType = "_airwave._tcp."
    private var isDiscovering = false

    private val discoveryListener = object : NsdManager.DiscoveryListener {
        override fun onDiscoveryStarted(regType: String) {
            isDiscovering = true
            Log.d("DiscoveryManager", "Service discovery started: $regType")
        }

        override fun onServiceFound(service: NsdServiceInfo) {
            Log.d("DiscoveryManager", "Service found: ${service.serviceName}")
            if (service.serviceType.contains("_airwave")) {
                try {
                    nsdManager.resolveService(service, resolveListener)
                } catch (e: Exception) {
                    Log.e("DiscoveryManager", "Resolve failed: ${e.message}")
                }
            }
        }

        override fun onServiceLost(service: NsdServiceInfo) {
            Log.d("DiscoveryManager", "Service lost: ${service.serviceName}")
        }

        override fun onDiscoveryStopped(serviceType: String) {
            isDiscovering = false
            Log.d("DiscoveryManager", "Discovery stopped: $serviceType")
        }

        override fun onStartDiscoveryFailed(serviceType: String, errorCode: Int) {
            isDiscovering = false
            Log.e("DiscoveryManager", "Start discovery failed: $errorCode")
        }

        override fun onStopDiscoveryFailed(serviceType: String, errorCode: Int) {
            Log.e("DiscoveryManager", "Stop discovery failed: $errorCode")
        }
    }

    private val resolveListener = object : NsdManager.ResolveListener {
        override fun onResolveFailed(serviceInfo: NsdServiceInfo, errorCode: Int) {
            Log.e("DiscoveryManager", "Resolve failed with error code: $errorCode")
        }

        override fun onServiceResolved(serviceInfo: NsdServiceInfo) {
            val host = serviceInfo.host.hostAddress ?: return
            val port = serviceInfo.port
            val name = serviceInfo.serviceName
            Log.i("DiscoveryManager", "Discovered Airwave Host: $name @ $host:$port")
            onDeviceFound(name, host, port)
        }
    }

    fun startDiscovery() {
        if (!isDiscovering) {
            try {
                nsdManager.discoverServices(serviceType, NsdManager.PROTOCOL_DNS_SD, discoveryListener)
            } catch (e: Exception) {
                Log.e("DiscoveryManager", "Cannot start discovery: ${e.message}")
            }
        }
    }

    fun stopDiscovery() {
        if (isDiscovering) {
            try {
                nsdManager.stopServiceDiscovery(discoveryListener)
            } catch (e: Exception) {
                Log.e("DiscoveryManager", "Cannot stop discovery: ${e.message}")
            }
        }
    }
}
