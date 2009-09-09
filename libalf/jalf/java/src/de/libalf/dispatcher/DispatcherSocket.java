package de.libalf.dispatcher;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

import de.libalf.Knowledgebase.Acceptance;

class DispatcherSocket {
	private DataInputStream in;
	private DataOutputStream out;

	public DispatcherSocket(Socket socket) throws DispatcherIOException {
		try {
			this.in = new DataInputStream(socket.getInputStream());
			this.out = new DataOutputStream(socket.getOutputStream());
		} catch (IOException e) {
			throw new DispatcherIOException(e);
		}
	}

	@Deprecated
	public DispatcherSocket(Process process) {
		this.in = new DataInputStream(process.getInputStream());
		this.out = new DataOutputStream(process.getOutputStream());
	}

	////////////////////////////////////////////////////////////////
	// SENDING

	private void flush() {
		try {
			this.out.flush();
		} catch (IOException e) {
			throw new DispatcherIOException(e);
		}
	}

	private void writeSendable(Sendable o) {
		writeInt(o.getInt());
	}

	private void writeBool(boolean b) {
		writeInt(b ? 1 : 0);
	}

	private void writeInt(int i) {
		try {
			this.out.writeInt(i);
		} catch (IOException e) {
			throw new DispatcherIOException(e);
		}
	}

	private void writeInts(int[] is) {
		writeInt(is.length);
		for (int i : is)
			writeInt(i);
	}

	int writeCommand(DispatcherConstants cmd, Object... args) throws DispatcherIOException {
		// send command
		writeSendable(cmd);

		// send arguments
		for (Object arg : args) {
			if (arg instanceof Sendable)
				writeSendable((Sendable) arg);
			else if (arg instanceof Boolean)
				writeBool((Boolean) arg);
			else if (arg instanceof Integer)
				writeInt((Integer) arg);
			else if (arg instanceof int[])
				writeInts((int[]) arg);
			else
				throw new IllegalArgumentException();
		}
		
		flush();

		// get response code
		return readInt();
	}

	int getSize(Object... args) {
		int size = 0;
		for (Object arg : args) {
			if (arg instanceof Sendable)
				size++;
			else if (arg instanceof Boolean)
				size++;
			else if (arg instanceof Integer)
				size++;
			else if (arg instanceof int[])
				size += 1 + ((int[]) arg).length;
			else
				throw new IllegalArgumentException();
		}
		return size;
	}

	void writeCommandThrowing(DispatcherConstants cmd, Object... args) throws DispatcherIOException, DispatcherProtocolException {
		int result = writeCommand(cmd, args);
		if (result == 0)
			return;
		
		System.out.println(DispatcherError.toString(result));
		throw cmd.getException();
	}

	////////////////////////////////////////////////////////////////
	// RECEIVING

	public boolean readBool() throws DispatcherIOException {
		return readInt() != 0;
	}

	public int readInt() throws DispatcherIOException {
		try {
			return this.in.readInt();
		} catch (IOException e) {
			throw new DispatcherIOException(e);
		}
	}

	public int[] readInts() throws DispatcherIOException {
		int[] is = new int[readInt()];
		for (int i = 0; i < is.length; i++)
			is[i] = readInt();
		return is;
	}

	private byte readByte() {
		try {
			return this.in.readByte();
		} catch (IOException e) {
			throw new DispatcherIOException(e);
		}
	}

	@SuppressWarnings("deprecation")
	public String readString() throws DispatcherIOException {
		byte[] buf = new byte[readInt()];
		for (int i = 0; i < buf.length; i++)
			buf[i] = readByte();
		assert is7bit(buf);
		return new String(buf, 0);
	}

	private boolean is7bit(byte[] buf) {
		for (byte b : buf)
			if (b < 0)
				return true;
		return true;
	}

	public Acceptance readAcceptance() {
		boolean known = readBool();
		boolean accept = readBool();
		return known ? accept ? Acceptance.ACCEPT : Acceptance.REJECT : Acceptance.UNKNOWN;
	}
}
