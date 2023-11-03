// store.js

import { createStore } from "vuex";

// Create a new Vuex store instance
const store = createStore({
	state: {
		// Your application's state variables
		// For example:
		isConnected: null,

		// ...other state variables
	},
	mutations: {
		// Functions to directly mutate the state
		changeConnectState(state, newState) {
			state.isConnected = newState;
		},
		changeToken(state, newToken) {
			state.token = newToken;
		},
		// ...other mutation functions
	},
	actions: {
		// Actions can contain asynchronous operations

		connected(context) {
			context.commit("changeConnectState", true);
		},
		disconnected(context) {
			context.commit("changeConnectState", false);
		},
		// ...other action functions
	},
	getters: {
		// Getters are used to compute derived state based on the store's state
		// For example:
		getConnectionStatus(state) {
			return state.isConnected;
		},
		getToken(state) {
			return state.token;
		},
		// ...other getter functions
	},
});

export default store;
