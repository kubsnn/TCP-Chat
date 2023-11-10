<script setup>
import ToastOverlay from './components/ToastOverlay.vue'
</script >

<template>
  <ToastOverlay />
  <main class="absolute top-0 left-0 h-screen w-screen bg-zinc-100 dark:bg-gray-900 z-0">
    <router-view></router-view>
  </main>
</template>
