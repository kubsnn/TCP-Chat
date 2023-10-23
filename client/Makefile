
all:

build:
	@echo "Building client..."
	@cd TCPChatClient/web-src && npm install
	@cd TCPChatClient/web-src && npm run build


run:
	@echo "Running client..."
	@cd python TCPChatClient

clean:
	@echo "Cleaning client..."
	@cd TCPChatClient/web-src && rm -rf node_modules
	@cd TCPChatClient/web-src && rm -rf build
	@cd TCPChatClient/web-src && rm -rf package-lock.json
	@cd TCPChatClient/web-src && rm -rf yarn.lock
	@cd TCPChatClient/web-src && rm -rf .yarn-integrity
	@cd TCPChatClient/web-src && rm -rf .yarnrc.yml
	@cd TCPChatClient/web-src && rm -rf .yarn
	@cd TCPChatClient/web-src && rm -rf .pnp.js
	@cd TCPChatClient/web-src && rm -rf .pnp.cjs
	@cd TCPChatClient/web-src && rm -rf .yarnrc